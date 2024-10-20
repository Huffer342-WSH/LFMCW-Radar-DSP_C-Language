#include "radar_cfar.h"
#include <stdint.h>
#include <stdio.h>

#define LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
// #define LOG(fmt, ...)

#if 0 // 选择unwrapper方式，x86 if更快
static inline int unwrapper_neg(int x, int n)
{
    return x < 0 ? x + n : x;
}
static inline int unwrapper_pos(int x, int n)
{
    return x >= n ? x - n : x;
}
#else
static inline int unwrapper_neg(int x, int n)
{
    return (x + n) % n;
}
static inline int unwrapper_pos(int x, int n)
{
    return x % n;
}
#endif

/**
 * @brief 2D CFAR算法
 * 
 
 * @param amp 输入数据
 * @param cfar2d_result_t  检测结果
 * @param num0  0维度的大小
 * @param num1  1维度的大小
 * @param train0  0维度的训练单元大小
 * @param train1  1维度的训练单元大小
 * @param guard0  0维度的保护单元大小
 * @param guard1  1维度的保护单元大小
 * @param thSNR  CFAR阈值-信噪比
 * @param thAmp  CFAR阈值-幅度
 * @return 0 - success
 * 
 * @details 每一个单元的噪声水平等于其上下左右四个训练单元里里的最大值。
 *          信噪比为幅度/噪声水平
 *          同时满足幅度大于thAmp和信噪比大于thSNR的点添加到检测结果中
 */
int radardsp_cfar2d(const double *amp, cfar2d_result_t *res, uint32_t num0, uint32_t num1,
                    uint32_t train0, uint32_t train1, uint32_t guard0, uint32_t guard1,
                    double thSNR, double thAmp)
{
    /* 在实际应用中，上百个点中往往只有几个点可以满足要求，所以是没有必要完整计算所有内容的
        建议先判断thAmp和一个训练的幅值，基本上和可以排除掉99%的点了。
        然后再计算剩下三个训练单元的大小，剩下三个就不需要每一步都用if判断了，因为可以排除掉的可能性变小了，每次都if判断反而会浪费时间
     */


    /* 根据DFT的性质，可知频谱时周期性的，速度维度的训练单元循环使用，即0的左边左右边的单元*/
    int ret = 0;
    double(*amp2D)[num1] = (double(*)[num1])amp; // 强制类型转换成二维数组 \
                                                (需要支持VLA的编译器，如AC6、gcc)
    res->numPoint = 0;

    uint8_t enable_sliding_window = train1 >= 2; // 是否启用滑动窗口,训练单元为1时滑动窗口速度无提升
    int total1 = guard1 + train1, g1 = guard1, g0 = guard0, t0 = train0, t1 = train1;
    /**
    * @brief 双重循环遍历
    */
    for (int i0 = 0; i0 < num0; i0++) { // 距离维度循环

        int cneter = num1; // 滑动窗口中心位置。
        for (int _i1 = -(int)num1 / 2; _i1 < ((int)num1 / 2); _i1++) {
            //速度维度循环，从[-Vmax,Vmax]循环
            int i1 = _i1 < 0 ? _i1 + num1 : _i1; //映射到[0,N-1]

            double a = amp2D[i0][i1];
            double sumLeft, sumRight;
            LOG("i0=%d\ti1=%d\ta=%f\t", i0, i1, a);
            if (a < thAmp) {
                // 跳过幅度小于幅度阈值的点
                LOG("\n");
                continue;
            } else if ((enable_sliding_window == 0) || ((cneter + 1) != _i1)) {
                // 和滑动窗口不相邻,重新初始化滑动窗口。或者没有使能滑动窗口时，一直会重新初始化
                sumLeft = sumRight = 0;
                cneter = _i1;
                for (int j = i1 - total1; j < i1 - g1; j++) {
                    sumLeft += amp2D[i0][unwrapper_neg(j, num1)]; // DFT的周期性
                }
                for (int j = i1 + g1 + 1; j <= i1 + g1 + t1; j++) {
                    sumRight += amp2D[i0][unwrapper_pos(j, num1)];
                }
            } else {
                // 和滑动窗口相邻,滑动窗口移动
                int add, sub;

                add = unwrapper_neg(i1 - g1 - 1, num1);
                sub = unwrapper_neg(add - t1, num1);
                sumLeft += (amp2D[i0][add] - amp2D[i0][sub]);

                sub = unwrapper_pos(i1 + g1, num1);
                add = unwrapper_pos(sub + t1, num1);
                sumRight += (amp2D[i0][add] - amp2D[i0][sub]);

                cneter = _i1;
            }
            LOG("left=%f\tright=%f\t", sumLeft, sumRight);
            // 比较横向的阈值
            double th = a / thSNR;
            double mean1 = (sumLeft > sumRight ? sumLeft : sumRight) / (double)t1;
            if (mean1 > th) { // 加入速度维度的噪声已经过大，就不需要继续计算了
                LOG("\n");
                continue;
            }

            // 计算纵向的噪声均值，纵向不循环移位
            double sumUp = 0, sumDown = 0;
            int start = i0 - g0 - t0;
            int end = start + t0;
            if (start < 0)
                start = 0;
            for (int j = start; j < end; j++) {
                sumUp += amp2D[j][i1];
            }
            start = i0 + g0 + 1;
            end = start + t0;
            if (end >= num0)
                end = num0;
            for (int j = start; j < end; j++) {
                sumDown += amp2D[j][i1];
            }
            double mean = (sumUp > sumDown ? sumUp : sumDown) / (double)t0;
            mean = mean > mean1 ? mean : mean1;
            if (mean <= th) { //添加点
                if (res->numPoint < res->numMax) {
                    cfar2d_point_t *p = &res->point[res->numPoint];
                    p->idx0 = i0;
                    p->idx1 = i1;
                    p->amp = a;
                    p->snr = a / mean;
                    res->numPoint++;
                } else {
                    // 超出最大个数
                    ret--;
                }
            }
            LOG("up=%f\tdown=%f", sumUp, sumDown);
            LOG("\n");
        }
    }
    return ret;
}

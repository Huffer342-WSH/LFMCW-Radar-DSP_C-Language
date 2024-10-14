#include "radar_cfar.h"
#include <stdint.h>


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

    range - doppler map
    /* 根据DFT的性质，可知频谱时周期性的，速度维度的训练单元循环使用，即0的左边左右边的单元*/
}

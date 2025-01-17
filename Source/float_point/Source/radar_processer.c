/**
 * @file lfmcw_radar_processer.c
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "radar_processer.h"
#include "radar_cfar.h"
#include <assert.h>
#include <stdlib.h>
#include <math.h>

int radardsp_init(radar_handle_t *radar)
{
    /*
    初始化radar里面的值，有需要的参数直接从外部传入
    */
    radar_param_t *param = &radar->param;
    radar_basic_data_t *basic = &radar->basic;

    param->numChannel = 2;
    param->numChrip = 32;
    param->numRangeBin = 25;

    basic->param = param;
    basic->staticClutter = malloc(sizeof(double) * 2 * param->numChannel * param->numRangeBin);
    // basic->staticClutter = NULL;
    basic->numStaticClutterAcc = 0;
    basic->staticClutterAccBuffer =
        malloc(sizeof(double) * 2 * param->numChannel * param->numRangeBin);
    basic->magSpec2D = malloc(sizeof(double) * param->numRangeBin * param->numChrip);
    return 0;
}

int radardsp_input_new_frame(radar_handle_t *radar, void *data)
{
    assert(data != NULL);
    assert(radar->basic.staticClutter != NULL);
    /*
    输入一帧RDM（2D-FFT后的产物）
    RDM的两个维度是(距离，速度)，速度是没有结果fftshift的，所以说后一半是负速度，前一半是正速度
    */


    int shape0 = radar->param.numRangeBin;
    int shape1 = radar->param.numChrip * 2;
    // double(*rdms)[shape0][shape1][2] = (double(*)[shape0][shape1][2])data;

    /* 1. 更新静态杂波，并减去静态杂波 */
    {
        uint32_t numCH = radar->param.numChannel;
        uint32_t numRB = radar->param.numRangeBin;
        uint32_t numChrip = radar->param.numChrip;
        double *pSrc = (double *)data;
        double *pDest = radar->basic.staticClutter;
        double *pEnd = pDest + numCH * numRB * 2;
        while (pDest < pEnd) {
            // 更新静态杂波
            *pDest = (*pDest + *pSrc) / 2;
            *(pDest + 1) = (*(pDest + 1) + *(pSrc + 1)) / 2;
            // *pDest = *pSrc;
            // *(pDest + 1) = *(pSrc + 1);
            // 减去静态杂波
            *pSrc -= *pDest;
            *(pSrc + 1) -= *(pDest + 1);

            pSrc += numChrip * 2;
            pDest += 2;
        }
    }



    /* 2. 计算幅度谱 */
    {
        double *pSrc0 = (double *)data;
        double *pSrc1 = (double *)data + radar->param.numRangeBin * radar->param.numChrip * 2;
        double *pDest = radar->basic.magSpec2D;
        double *pEnd = pDest + radar->param.numRangeBin * radar->param.numChrip;
        while (pDest < pEnd) {
            double real, imag;
            real = *pSrc0++;
            imag = *pSrc0++;
            *pDest++ = sqrt(real * real + imag * imag);
        }
    }


    /* 3. CFAR搜索点，最终输出的检测结果包含点的 */


    /* 4. 第一次聚类，在CFAR的数组坐标的基础只在速度维度上聚类。因为往往一个目标有多种速度成分，导致速度维度能量很分散 */


    /* 5. 计算极坐标，第二次聚类。第二次是在空间平面上聚类，加入到这一步的时候点云已经很少了，可以不聚类 */


    /* 到此为止，信号处理已经结束，得到的是新一帧的目标检测结果 
       接下来主要是完成目标跟踪，也就是把不同帧检测出来的结果前后关联起来，得到一个个目标的运动轨迹
    */


    /* 6. 目标更新：包含已有目标的关联或者删除，以及添加新的目标 
        关联： 使用目标在上一帧的位置，或者是预测出来的这一帧可能的位置 和 这一帧检测出来的目标匹配，将匹配上的点加入到已有的轨迹中 
        推测： 一个正在跟踪的目标没有成功匹配到新一帧目标时，不立马删除，开始推测（直接采用上一帧的预测值）。
        删除： 加入一个目标长时间没有成功匹配，或者关联后目标离开检测范围，就将这个目标删除。
        添加： 新出现的目标没有被关联过，就添加进来

        当使用卡尔曼滤波时，新添加的目标作为新的量测值用于纠正预测值，得到滤波后的值

        考虑使用打分机制维护目标的删除。 
        新目标需要有一个起始阶段，因为收到噪声干扰雷达会检测到一些假目标，这些假目标需要在没关联上时快速删除
        新添加进来的目标给一个初始分数，如果成功关联上就快速加分，否则就扣分，这样可以把假目标快速删掉；
        同时分数要设置上限，防止分数太高导致删不掉；
        目标处于静止状态时加分要减少
    */

    /* 7. 目标预测（可选）：根据已有的轨迹，预测下一帧的目标位置。
        一方面预测一帧的时间，可以在下一帧关联时更容易匹配到正确的目标
        另一方面，目标检测在低信噪比时会时隐时现，常常需要预测几帧数据。

        当使用卡尔曼滤波时，这里就用与实现卡尔曼预测
   */

    /*
        注意，为了避免非线性关系，坐标采用雷达信号处理出来的极坐标，而不是使用直角坐标系。
        因为在卡尔曼滤波中，状态转移方程是用一个矩阵相乘来表示的，加入观测值是直角，但是量测值是极坐标，就会导致状态转移方程是非线性的，需要使用扩展卡尔曼。
        （量测值：测出来的原始数据。  观测值：滤波时维护的数据）

        假如只使用直角坐标，相当于丢失了观测噪声在极坐标中的信息，比如测角精度固定，那越近直角坐标就越准吗。本来观测噪声是在极坐标下的，在经过观测矩阵后就可以把这个性质表达到直角坐标中。如果直接使用直角坐标系，就体现不出这个性质。
        
    */
    return 0;
}

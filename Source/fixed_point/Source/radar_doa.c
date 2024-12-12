/**
 * @file radar_doa.c
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 根据CFAR结果和RDM计算目标角度
 * @version 0.1
 * @date 2024-11-20
 *
 * @copyright Copyright (c) 2024
 *
 */


#include "radar_doa.h"
#include "radar_cfar.h"
#include "radar_assert.h"

#include <stdlib.h>
/**
 * @brief 输入两个复数，返回相位差
 *
 * @param real0     复数0-实部
 * @param imag0     复数0-虚部
 * @param real1     复数1-实部
 * @param imag1     复数1-虚部
 * @return int16_t  相位差，Q2.13弧度
 *
 */
static inline int16_t radar_doa_calc_phasediff_i16(int16_t real0, int16_t imag0, int16_t real1, int16_t imag1)
{
    int32_t phasediff_q13 = (int32_t)radar_atan2_q15(imag0, real0) - (int32_t)radar_atan2_q15(imag1, real1);
    if (phasediff_q13 < -PI_Q13) {
        phasediff_q13 += 2 * PI_Q13;
    }
    if (phasediff_q13 > PI_Q13) {
        phasediff_q13 -= 2 * PI_Q13;
    }
    return phasediff_q13;
}


/**
 * @brief  双通道雷达测角。 计算cfar结果中每一个点对应的角度，并保存到量测值列表中
 *
 * @param[out]    meas                  量测值列表，保存计算结果
 * @param[in,out] cfar                  CFAR结果，输入待测角点的坐标
 * @param[in]     rdm                   RDM数组，用与查询信号，计算角度
 * @param[in]     lambda_over_d_q15     波长/天线间距,Q16.15定点数
 * @param[in]     phase_diff_threshold  相位差阈值，使用Q2.13弧度，取值范围[0,PI]
 * @param[in]     mag_threshold         幅度差阈值，使用Q0.15定点数，取值范围[0,1)
 * @return int
 */
int radar_dual_channel_clac_angle(measurements_t *meas, cfar2d_result_t *cfar, matrix3d_complex_int16_t *rdms, int32_t lambda_over_d_q15,
                                  int16_t phase_diff_threshold, int16_t mag_threshold)
{
    RADAR_ASSERT(meas->capacity >= cfar->numPoint);

    const size_t offsetChannel = rdms->tda1 * 2;


    size_t count = 0;
    for (size_t i = 0; i < cfar->numPoint; i++) {

        size_t offset;
        int16_t real0, real1, imag0, imag1;
        offset = (cfar->point[i].idx0 * rdms->tda2 + cfar->point[i].idx1) * 2;
        real0 = rdms->data[offset];
        imag0 = rdms->data[offset + 1];
        offset += offsetChannel;
        real1 = rdms->data[offset];
        imag1 = rdms->data[offset + 1];

        /* 校验幅度差 */
        int64_t mag0 = real0 * real0 + imag0 * imag0;
        int64_t mag1 = real1 * real1 + imag1 * imag1;
        int64_t mag_diff_normalized = (abs_diff(mag0, mag1) << 15) / (mag0 + mag1);

        if (mag_diff_normalized > mag_threshold) {
            continue;
        }


        /* 校验相位差 */
        int16_t delta_phase = radar_doa_calc_phasediff_i16(real0, imag0, real1, imag1);
        if (abs(delta_phase) > phase_diff_threshold) {
            continue;
        }


        /* 符合条件,计算角度 */
        int16_t sin_theta = (int32_t)delta_phase * lambda_over_d_q15 / ((int32_t)2 * PI_Q13);
        int16_t theta = radar_asin_q15(sin_theta);
        if (count != i) {
            cfar->point[count] = cfar->point[i];
        }
        meas->data[count].azimuth = theta;
        count++;
    }
    cfar->numPoint = count;
    meas->num = count;

    return 0;
}

#include "radar_doa.h"
#include "radar_cfar.h"

/**
 * @brief 输入两个复数，返回相位差
 *
 * @param real0 复数0-实部
 * @param imag0 复数0-虚部
 * @param real1 复数1-实部
 * @param imag1 复数1-虚部
 * @return int16_t
 * 
 * @details 可以先算
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
 * @param meas 量测值列表
 * @param cfar CFAR结果
 * @param rdm  RDM数组
 * @return int
 */
int radar_dual_channel_clac_angle(measurements_t *meas, cfar2d_result_t *cfar, matrix3d_complex_int16_t *rdms, int32_t lambda_over_d_q15)
{

    const size_t offsetChannel = rdms->tda1 * 2;
    int16_t real0, real1, imag0, imag1;
    for (size_t i = 0; i < cfar->numPoint; i++) {
        size_t offset;
        offset = (cfar->point[i].idx0 * rdms->tda2 + cfar->point[i].idx1) * 2;
        real0 = rdms->data[offset];
        imag0 = rdms->data[offset + 1];
        offset += offsetChannel;
        real1 = rdms->data[offset];
        imag1 = rdms->data[offset + 1];
        int16_t delta_phase = radar_doa_calc_phasediff_i16(real0, imag0, real1, imag1);
        int16_t sin_theta = (int32_t)delta_phase * lambda_over_d_q15 / ((int32_t)2 * PI_Q13);
        int16_t theta = radar_asin_q15(sin_theta);


        meas->data[i].azimuth = theta;
    }
    return 0;
}

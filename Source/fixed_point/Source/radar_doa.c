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
}

/**
 * @brief  双通道雷达测角。 计算cfar结果中每一个点对应的角度，并保存到量测值列表中
 *
 * @param meas 量测值列表
 * @param cfar CFAR结果
 * @param rdm  RDM数组
 * @return int
 */
int radar_dual_channel_clac_angle(radar_measurement_list_t *meas, cfar2d_result_t *cfar, matrix2d_complex_int16_t *rdm)
{
    return 0;
}

#pragma once

#include "radar_types.h"

#ifdef __cplusplus
extern "C" {
#endif


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
                                  int16_t phase_diff_threshold, int16_t mag_threshold);

#ifdef __cplusplus
}
#endif

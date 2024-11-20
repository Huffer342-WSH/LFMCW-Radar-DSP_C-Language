/**
 * @file radar_dis_velo.h
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 根据CFAR结果计算距离与速度
 * @version 0.1
 * @date 2024-11-20
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "radar_math_types.h"
#include "radar_cfar.h"
#include "radar_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief 计算速度与距离
 *
 * @param meas 量测值列表
 * @param cfar cfar结果
 * @param mag 幅度谱
 * @param resRange 距离分辨率
 * @param resVel 速度分辨率
 * @return int
 */
int radar_clac_dis_and_velo(measurements_t *meas, const cfar2d_result_t *cfar, const matrix2d_int32_t *mag, int32_t resRange, int32_t resVel);


#ifdef __cplusplus
}
#endif

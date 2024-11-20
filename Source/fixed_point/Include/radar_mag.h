/**
 * @file radar_mag.h
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 从RDM计算二维幅度谱
 * @version 0.1
 * @date 2024-11-20
 *
 * @copyright Copyright (c) 2024
 *
 */


#pragma once

#include "radar_math.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief 幅度谱计算，累加数个通道的RDM的幅度谱
 *
 * @param[out]  mag            幅度谱  mag = sqrt(real*real + imag*imag) * sqrt(2^25)
 * @param[in]   rdms           多个通道的RDM, 2DFFT后的产物
 * @param[in]   numChannel     累加的通道数
 * @param[in]   offsetChannel  通道数偏移
 *
 * @return int  0 - success
 */
int radar_clac_magSpec2D(matrix2d_int32_t *mag, matrix3d_complex_int16_t *rdms, uint16_t numChannel, uint16_t rdmOffset);


#ifdef __cplusplus
}
#endif

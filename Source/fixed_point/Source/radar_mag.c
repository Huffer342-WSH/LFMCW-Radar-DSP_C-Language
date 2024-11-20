/**
 * @file radar_mag.c
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 从RDM计算二维幅度谱
 * @version 0.1
 * @date 2024-11-20
 *
 * @copyright Copyright (c) 2024
 *
 */


#include "radar_mag.h"

#include "radar_assert.h"

#include <string.h>

/**
 * @brief 幅度谱计算，累加数个通道的RDM的幅度谱
 *
 * @details mag是 rdms[offsetChannel]到rdms[offsetChannel + numChannel - 1]的幅度谱累加的结果
 *
 * @param[out]  mag            幅度谱  mag = sqrt(real*real + imag*imag) * sqrt(2^25)
 * @param[in]   rdms           多个通道的RDM, 2DFFT后的产物
 * @param[in]   numChannel     累加的通道数
 * @param[in]   offsetChannel  通道数偏移
 *
 * @return int  0 - success
 */
int radar_clac_magSpec2D(matrix2d_int32_t *mag, matrix3d_complex_int16_t *rdms, uint16_t numChannel, uint16_t offsetChannel)
{
    RADAR_ASSERT(mag != NULL && rdms != NULL && mag->size0 == rdms->size1 && mag->size1 == rdms->size2 && numChannel + offsetChannel <= rdms->size0);

    const size_t magSize = mag->size0 * mag->size1;
    const size_t rdmSize = 2 * rdms->size1 * rdms->size2;

    int32_t *pMag;
    int16_t *pRdms = rdms->data + offsetChannel * rdmSize;

    memset(mag->data, 0, magSize * sizeof(int32_t));
    for (size_t i = 0; i < numChannel; i++) {
        pMag = mag->data;
        for (size_t j = 0; j < magSize; j++) {
            int16_t real, imag;
            int32_t square;
            real = *pRdms++;
            imag = *pRdms++;
            square = (int32_t)real * real + (int32_t)imag * imag;
            square = *pMag++ += radar_sqrt_q31(square) >> 3;
        }
    }
    return 0;
}

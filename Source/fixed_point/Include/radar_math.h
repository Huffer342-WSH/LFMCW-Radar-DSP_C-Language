/**
 * @file radar_math.h
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 数学函数, 可以自己实现，默认使用CMSIS-DSP库中的函数
 * @version 0.1
 * @date 2024-11-20
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include <stdint.h>
#include "radar_log.h"
#include "radar_math_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "arm_math.h"

#ifndef PI
#define PI 3.14159265358979f
#endif

#ifndef PI_F64
#define PI_F64 3.14159265358979323846
#endif

#ifndef PI_Q13
#define PI_Q13 25736
#endif

#define LIGHT_SPEED (299792458)

static inline int32_t radar_sqrt_q31(int32_t x)
{
    int32_t y;
    arm_sqrt_q31(x, &y);
    return y;
}

static inline int16_t radar_cos_q15(int16_t x)
{
    return arm_cos_q15(x);
}


static inline int32_t radar_cos_q31(int32_t x)
{
    return arm_cos_q31(x);
}


static inline int16_t radar_atan2_q15(int16_t y, int16_t x)
{
    int16_t angle;
    arm_atan2_q15(y, x, &angle);
    return angle;
}


static inline int32_t radar_atan2_q31(int32_t y, int32_t x)
{
    int32_t angle;
    arm_atan2_q31(y, x, &angle);
    return angle;
}


// arcsin = arctan(x/sqrt(1-x^2))

/**
 * @brief
 * @param x x ∈ [-1, 1) = {-}
 *
 * @return int16_t
 */
static inline int16_t radar_asin_q15(int16_t x)
{
    int32_t x_q31 = ((int32_t)x << 16);
    uint32_t xx_q31 = (uint32_t)((int32_t)x * x) << 1;
    int32_t y_q31 = (int32_t)radar_sqrt_q31((int32_t)(((uint32_t)1 << 31) - xx_q31));
    int32_t theta_q29 = radar_atan2_q31(x_q31, y_q31);
    return theta_q29 >> 16;
}


static inline int32_t mult_i32_i32q16_i32(int32_t a, int32_t b)
{
    return (int64_t)a * b >> 16;
}

static inline int32_t div_i32q16_i32q16(int32_t a, int32_t b)
{
    return (int64_t)a * (1 << 16) / b;
}


#define abs_diff(a, b) ((a) > (b) ? (a) - (b) : (b) - (a))


#ifdef __cplusplus
}
#endif

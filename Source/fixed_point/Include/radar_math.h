#ifndef _RADAR_MATH_H_
#define _RADAR_MATH_H_
#include <stdint.h>
#include <math.h>

#include "radar_math_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "arm_math.h"


static inline int32_t radar_sqrt_q31(int32_t x)
{
    int32_t y;
    arm_sqrt_q31(x, &y);
    return y;
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
#endif /* _RADAR_MATH_H_ */

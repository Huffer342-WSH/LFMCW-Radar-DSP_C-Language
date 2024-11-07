#ifndef _RADAR_MATH_H_
#define _RADAR_MATH_H_
#include <stdint.h>
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


#ifdef __cplusplus
}
#endif
#endif /* _RADAR_MATH_H_ */

#ifndef _RADAR_DIS_VELO_H_
#define _RADAR_DIS_VELO_H_

#include "radar_math_types.h"
#include "radar_cfar.h"
#include "radar_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int radar_clac_dis_and_velo(measurements_t *meas, const cfar2d_result_t *cfar, const matrix2d_int32_t *mag, int32_t resRange, int32_t resVel);


#ifdef __cplusplus
}
#endif
#endif /* _RADAR_DIS_VELO_H_ */

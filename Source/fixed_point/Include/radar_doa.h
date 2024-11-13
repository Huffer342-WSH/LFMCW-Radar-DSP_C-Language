#ifndef _RADAR_DOA_H_
#define _RADAR_DOA_H_

#include "radar_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int radar_dual_channel_clac_angle(radar_measurement_list_fixed_t *meas, cfar2d_result_t *cfar, matrix3d_complex_int16_t *rdms);

#ifdef __cplusplus
}
#endif
#endif /* _RADAR_DOA_H_ */

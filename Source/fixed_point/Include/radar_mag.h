#ifndef _RADAR_MAG_H_
#define _RADAR_MAG_H_

#include "radar_math_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int radar_clac_magSpec2D(matrix2d_int32_t *mag, matrix2d_complex_int16_t *rdm, uint16_t numChannel, uint16_t rdmOffset);

#ifdef __cplusplus
}
#endif
#endif /* _RADAR_MAG_H_ */

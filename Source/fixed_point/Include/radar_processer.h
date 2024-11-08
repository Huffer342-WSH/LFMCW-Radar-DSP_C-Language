#pragma once

#include "radar_config.h"
#include "radar_types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    float wavelength;
    float bandwidth;
    float timeChrip;
    float timeChripGap;
    float timeFrameGap;
    uint16_t numChannel;
    uint16_t numRangeBin;
    uint16_t numChrip;
} radar_init_param_t;

int radardsp_init(radar_handle_t *radar, radar_init_param_t *param);

int radardsp_input_new_frame(radar_handle_t *radar, matrix3d_complex_int16_t *rdms);


#ifdef __cplusplus
}
#endif

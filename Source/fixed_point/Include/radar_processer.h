#pragma once

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
    uint16_t numMaxCfarPoints;
} radar_init_param_t;


int radardsp_init(radar_handle_t *radar, radar_init_param_t *param, radar_config_t *config);

int radardsp_input_new_frame(radar_handle_t *radar, matrix3d_complex_int16_t *rdms);

void radardsp_register_hook_cfar_raw(radar_handle_t *radar, void (*func)(const cfar2d_result_t *));
void radardsp_register_hook_cfar_filtered(radar_handle_t *radar, void (*func)(const cfar2d_result_t *));
void radardsp_register_hook_point_clouds(radar_handle_t *radar, void (*func)(const measurements_t *));
void radardsp_register_hook_clusters(radar_handle_t *radar, void (*func)(const measurements_t *));


#ifdef __cplusplus
}
#endif

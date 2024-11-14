#include "radar_types.h"

#include <stdlib.h>


int radar_basic_data_init(radar_basic_data_t *basic, radar_param_t *param)
{

    basic->param = param;
    basic->rdms = radar_matrix3d_complex_int16_alloc(param->numChannel, param->numRangeBin, param->numChrip);
#if ENABLE_STATIC_CLUTTER_FILTERING == ON
    basic->staticClutter = radar_matrix2d_complex_int32_alloc(param->numChannel, param->numRangeBin);
    basic->staticClutterAccBuffer = radar_matrix2d_complex_int32_alloc(param->numChannel, param->numRangeBin);
#endif
    basic->magSpec2D = radar_matrix2d_int32_alloc(param->numRangeBin, param->numChrip);

    return 0;
}


radar_measurement_list_fixed_t *radar_measurement_list_alloc(size_t capacity)
{
    radar_measurement_list_fixed_t *meas = (radar_measurement_list_fixed_t *)malloc(sizeof(radar_measurement_list_fixed_t));
    meas->meas = (radar_measurements_fixed_t *)malloc(sizeof(radar_measurements_fixed_t) * capacity);
    meas->num = 0;
    meas->capacity = capacity;
    return meas;
}


void radar_measurement_list_free(radar_measurement_list_fixed_t *m)
{
    free(m->meas);
    free(m);
}

#include "radar_types.h"
#include "radar_error.h"

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

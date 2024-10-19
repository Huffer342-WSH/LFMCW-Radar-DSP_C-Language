#include <stdio.h>
#include <math.h>
#include <string.h>
#include "radar_cfar.h"
#include <stdlib.h>


#include "radar_processer.h"
extern void spec_clustering(cfar2d_result_t *cfar2d_result);

int main()
{
    printf("radar_dsp_float_point_test Test\n");

    cfar2d_result_t cfar2d_result;
    cfar2d_point_t cfar2d_result_point[5];
    uint32_t idx0_m[5] = { 8, 8, 8, 8, 10 };
    uint32_t idx1_m[5] = { 10, 11, 12, 10, 11 };
    double amp_m[5] = { 4.0, 7.0, 1.0, 4.0, 8.0 };
    double snr_m[5] = { 5.0, 5.0, 5.0, 5.0, 5.0 };

    for (int i = 0; i < 5; i++) {
        cfar2d_result_point[i].idx0 = idx0_m[i];
        cfar2d_result_point[i].idx1 = idx1_m[i];
        cfar2d_result_point[i].amp = amp_m[i];
        cfar2d_result_point[i].snr = snr_m[i];
    }


    cfar2d_result.point = cfar2d_result_point;
    cfar2d_result.numMax = 10;
    cfar2d_result.numPoint = 5;


    spec_clustering(&cfar2d_result);



    return 0;
}

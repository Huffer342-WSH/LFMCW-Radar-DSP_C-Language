#include <stdio.h>
#include <math.h>
#include <string.h>
#include "radar_cfar.h"
#include <stdlib.h>


#include "radar_processer.h"
extern void spec_clustering(cfar2d_result_t *cfar2d_result);
extern void cfar2d_del_point(cfar2d_result_t *list);


int main()
{
    printf("radar_dsp_float_point_test Test\n");

    cfar2d_result_t cfar2d_result;
    cfar2d_point_t cfar2d_result_point[10];
    uint32_t idx0_m[] = { 7, 8, 8, 8, 8, 10, 10 };
    uint32_t idx1_m[] = { 7, 10, 11, 12, 13, 11, 12 };
    double amp_m[] = { 1, 4.0, 7.0, 1.0, 4.0, 8.0, 9 };
    double snr_m[] = { 1, 5.0, 5.0, 5.0, 5.0, 5.0, 9 };

    int numPoint = sizeof(idx0_m) / sizeof(idx0_m[0]);
    for (int i = 0; i < numPoint; i++) {
        cfar2d_result_point[i].idx0 = idx0_m[i];
        cfar2d_result_point[i].idx1 = idx1_m[i];
        cfar2d_result_point[i].amp = amp_m[i];
        cfar2d_result_point[i].snr = snr_m[i];
    }


    cfar2d_result.point = cfar2d_result_point;
    cfar2d_result.numMax = 10;
    cfar2d_result.numPoint = numPoint;


    spec_clustering(&cfar2d_result);


    for (int i = 0; i < numPoint; i++) {
        cfar2d_result_point[i].idx0 = idx0_m[i];
        cfar2d_result_point[i].idx1 = idx1_m[i];
        cfar2d_result_point[i].amp = amp_m[i];
        cfar2d_result_point[i].snr = snr_m[i];
    }
    cfar2d_del_point(&cfar2d_result);

    puts("\n\ncfar2d_del_point() Test Result:");
    for (int i = 0; i < cfar2d_result.numPoint; i++) {
        printf("[idx0,idx1,amp]: [%d,%d,%f]\n", cfar2d_result.point[i].idx0,
               cfar2d_result.point[i].idx1, cfar2d_result.point[i].amp);
    }



    return 0;
}

#include <gtest/gtest.h>

#include "radar_cluster.h"

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cmath>

TEST(RadarFixedTest, radar_cluster_calc_distance)
{

    const int N = 4;
    const double WR = 1.0;
    const double WV = 1.0;
    double pos[][3] = { { 4330, 2500, 0 }, { 2500, 4330, 0 }, { 200, 100, 0 }, { 200, 200, 0 } };
    radar_measurement_list_fixed_t *m = radar_measurement_list_alloc(10);

    for (size_t i = 0; i < 4; i++) {
        m->meas[i].distance = sqrt(pos[i][0] * pos[i][0] + pos[i][1] * pos[i][1]) + 0.5;
        m->meas[i].azimuth = atan2(pos[i][1], pos[i][0]) * (double)(1 << 13) + 0.5;
        m->meas[i].velocity = pos[i][2];
    }
    m->num = N;

    int32_t *D = radar_cluster_calc_distance(m, WR * 65536, WV * 65536);
    for (size_t i = 1; i < N; i++) {
        for (size_t j = 0; j < i; j++) {
            printf("%6d ", D[i * (i - 1) / 2 + j]);
            double d, r, v;
            r = sqrt((pos[i][0] - pos[j][0]) * (pos[i][0] - pos[j][0]) + (pos[i][1] - pos[j][1]) * (pos[i][1] - pos[j][1]));
            v = pos[i][2] - pos[j][2];
            d = sqrt(r * r * WR * WR + v * v * WV * WV);
            v = printf("%6.2f\t |", d);
            ASSERT_NEAR(d, D[i * (i - 1) / 2 + j], 5);
        }
        printf("\n");
    }
    free(D);
    radar_measurement_list_free(m);
}
// 运行所有测试
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

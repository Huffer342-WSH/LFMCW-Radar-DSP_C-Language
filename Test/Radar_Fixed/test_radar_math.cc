#include <gtest/gtest.h>

#include "radar_math.h"

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cmath>

TEST(RadarFixedTest, radar_math_asin_q15)
{
    const int n = 2000;
    for (int i = -n; i < n; i++) {
        double x = (double)i / n;
        int16_t in = x * (1 << 15);
        double ref_f64 = asin(x);
        int16_t ref_q13 = ref_f64 * (1 << 13);
        int16_t out_q13 = radar_asin_q15(in);
        double out_f64 = (double)out_q13 / (1 << 13);
        ASSERT_NEAR(out_f64, ref_f64, 0.001);
        printf("arcsin(%.3lf) = | %.3lf, %.3lf | %6d, %6d |\n", x, ref_f64, out_f64, ref_q13, out_q13);
    }
}

// 运行所有测试
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

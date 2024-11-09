#include "radar_mag.h"
#include <gtest/gtest.h>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <random>


TEST(RadarFixedTest, radar_clac_magSpec2D)
{
    static int16_t in[2][20][50][2];
    static int32_t out[20][50];
    static int32_t ref[20][50];

    matrix3d_complex_int16_t rdms;
    matrix2d_int32_t mag;

    rdms.size0 = 2;
    rdms.size1 = 20;
    rdms.size2 = 50;
    rdms.tda1 = 20 * 50 * 2;
    rdms.tda2 = 50 * 2;
    rdms.data = (int16_t *)in;

    mag.data = (int32_t *)out;
    mag.size0 = 20;
    mag.size1 = 50;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int16_t> dis(-32768, 32767);

    const int repeat_count = 20; // 设置重复测试次数

    for (int repeat = 0; repeat < repeat_count; ++repeat) {
        // 生成随机数据
        for (size_t i = 0; i < 2; i++) {
            for (size_t j = 0; j < 20; j++) {
                for (size_t k = 0; k < 50; k++) {
                    in[i][j][k][0] = dis(gen);
                    in[i][j][k][1] = dis(gen);
                }
            }
        }

        // 清零参考和输出数组
        memset(ref, 0, sizeof(ref));
        memset(out, 0, sizeof(out));

        // 计算参考结果
        for (size_t i = 0; i < 2; i++) {
            for (size_t j = 0; j < 20; j++) {
                for (size_t k = 0; k < 50; k++) {
                    int64_t square;
                    square = (int64_t)((int32_t)in[i][j][k][0] * in[i][j][k][0]) + (int32_t)in[i][j][k][1] * in[i][j][k][1];
                    ASSERT_LT(square, ((int64_t)1 << 31) - 1);
                    ref[j][k] += (int32_t)(sqrt(square) * sqrt(1 << 25));
                }
            }
        }

        // 调用被测试函数
        radar_clac_magSpec2D(&mag, &rdms, 2, 0);

        // 比较结果
        for (size_t j = 0; j < 20; j++) {
            for (size_t k = 0; k < 50; k++) {
                for (size_t i = 0; i < 2; i++) {
                    printf("[%2lu, %2lu, %2lu]  %-8d %-8d  |", i, j, k, in[i][j][k][0], in[i][j][k][1]);
                    printf(" %+8d |", ref[j][k]);
                    printf(" %+8d |\n", out[j][k]);
                }
                ASSERT_NEAR(out[j][k], ref[j][k], 5);
            }
        }
    }
}

// 运行所有测试
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

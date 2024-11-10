#include <gtest/gtest.h>

#include "radar_cfar.h"

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <chrono>

class RadarFixedTest : public ::testing::Test
{
protected:
    static constexpr size_t repeat_times = 1 << 15; // 减小 repeat_times 以便测试运行
    static constexpr int n = 64;
    volatile int k; // volatile 防止优化
    int mask = n - 1;

    double measure_execution_time(const std::function<void()> &func)
    {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        return duration.count();
    }
};

TEST_F(RadarFixedTest, radar_cfar_unwrap)
{
    // 测试第一段代码
    double time1 = measure_execution_time([&]() {
        for (size_t repeat = 0; repeat < repeat_times; repeat++) {
            for (size_t i = n / 2; i < n * 3 / 2; i++) {
                k = i & mask;
            }
        }
    });

    // 测试第二段代码
    double time2 = measure_execution_time([&]() {
        for (size_t repeat = 0; repeat < repeat_times; repeat++) {
            for (size_t i = n / 2; i < n * 3 / 2; i++) {
                k = i % mask;
            }
        }
    });

    // 测试第三段代码
    double time3 = measure_execution_time([&]() {
        for (size_t repeat = 0; repeat < repeat_times; repeat++) {
            for (size_t i = n / 2; i < n * 3 / 2; i++) {
                k = i > n ? i - n : i;
            }
        }
    });

    std::cout << "Execution time for i & mask:          " << time1 << " seconds" << std::endl;
    std::cout << "Execution time for i % mask:          " << time2 << " seconds" << std::endl;
    std::cout << "Execution time for i > n ? i - n : i: " << time3 << " seconds" << std::endl;
}

TEST_F(RadarFixedTest, radar_cfar)
{
#define MAG(i, j) magSpec2D->data[(i) * magSpec2D->tda1 + (j)]
    const int M = 10, N = 8;
    matrix2d_int32_t *magSpec2D = radar_matrix2d_int32_alloc(M, N);
    cfar2d_result_t *res = cfar2d_result_alloc(50);
    cfar2d_cfg_t cfg;

    ASSERT_NE((uintptr_t)magSpec2D, NULL);
    ASSERT_NE((uintptr_t)res, NULL);

    memset(magSpec2D->data, 0, magSpec2D->size0 * magSpec2D->size1 * sizeof(int32_t));
    for (size_t i = 0; i < M; i++) {
        for (size_t j = 0; j < N; j++) {
            MAG(i, j) = 10;
        }
    }
    cfg.numGuard[0] = 2;
    cfg.numGuard[1] = 2;
    cfg.numTrain[0] = 2;
    cfg.numTrain[1] = 2;
    cfg.thSNR = 2;
    cfg.thAmp = 2;

    /* 1 */
    radar_cfar2d_goca(res, magSpec2D, &cfg);
    printf("%-30s %ld\n", "numPoint", res->numPoint);

    /* 2 */
    MAG(5, 7) = 50;
    radar_cfar2d_goca(res, magSpec2D, &cfg);
    printf("%-30s %ld\n", "numPoint", res->numPoint);
    ASSERT_EQ(res->point[0].idx0, 5);
    ASSERT_EQ(res->point[0].idx1, 7);


    radar_matrix2d_int32_free(magSpec2D);
    cfar2d_result_free(res);
#undef MAG
}
// 运行所有测试
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

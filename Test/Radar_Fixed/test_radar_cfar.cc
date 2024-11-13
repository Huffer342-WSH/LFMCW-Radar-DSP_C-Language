#include <gtest/gtest.h>

#include "radar_cfar.h"

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <chrono>
#include <random>

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


TEST_F(RadarFixedTest, radar_cfar_result_filtering)
{

    const size_t repeat_times = 100;
    const uint16_t M = 25, N = 64;
    int32_t map[M][N];
    cfar2d_result_t *res = cfar2d_result_alloc(200);
    cfar2d_filter_cfg_t cfg = { .range0 = 1, .range1 = 5, .shape1 = N, .thSNR = 0.78 };
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int32_t> dis(40000, 60000);
    for (size_t repeat = 0; repeat < repeat_times; repeat++) {
        /* 生成随机数据 */
        res->numPoint = 0;
        memset(map, 0, M * N * sizeof(int32_t));
        for (int k = 0; k < 10; k++) {
            int32_t a = dis(gen);
            int x = a % M;
            int y = a % N;
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < N; j++) {
                    map[i][j] += a / (abs(i - x) + abs(j - y) + 1);
                }
            }
        }
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                if (map[i][j] > 40000) {
                    cfar2d_result_add_point(res, i, j, map[i][j], 1024);
                }
            }
        }
        std::cout << "numPoint: " << res->numPoint << std::endl;

        radar_cfar_result_filtering(res, &cfg);


        /* 验证 */
        std::cout << "numPoint: " << res->numPoint << std::endl;
        for (int i = 0; i < res->numPoint; i++) {
            for (int j = 0; j < res->numPoint; j++) {
                if (i == j) {
                    continue;
                }
                cfar2d_point_t *a = &res->point[i];
                cfar2d_point_t *b = &res->point[j];
                int r0 = abs((int)a->idx0 - (int)b->idx0);
                int r1 = abs((int)a->idx1 - (int)b->idx1);
                if (r1 > N / 2)
                    r1 = N - r1;
                if (r0 <= cfg.range0 && r1 <= cfg.range1) {
                    printf("a: (%d,%d): %d\n", a->idx0, a->idx1, a->amp);
                    printf("b: (%d,%d): %d\n", b->idx0, b->idx1, b->amp);
                    ASSERT_GT(a->amp, b->amp * cfg.thSNR);
                }
            }
        }
    }
    cfar2d_result_free(res);
}


// TEST_F(RadarFixedTest, neg_mod)
// {

//     int32_t a = -1;
//     int32_t b = 8;
//     std::cout << a % b << std::endl;
// }


// 运行所有测试
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

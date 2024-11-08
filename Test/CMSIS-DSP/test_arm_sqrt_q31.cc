#include "arm_math.h"
#include <gtest/gtest.h>
#include <cmath>
#include <cstdint>

// 测试 arm_sqrt_q31 函数
TEST(CMSISDSPTest, arm_sqrt_q31)
{
    int32_t in[] = { 16 };
    int32_t out[sizeof(in) / sizeof(in[0])];
    int32_t ref[sizeof(in) / sizeof(in[0])];
    const size_t n = sizeof(in) / sizeof(in[0]);
    const int32_t epsilon = 5; // 设定误差范围，这个值可以根据需要调整

    // 执行 arm_sqrt_q31 运算，并生成参考值
    for (size_t i = 0; i < n; i++) {
        arm_sqrt_q31(in[i], &out[i]);
        ref[i] = (int32_t)(sqrt((double)in[i]) * sqrt(((int64_t)1 << 31)));
    }

    // 比较输出和参考值，允许一定的误差
    printf("Run Test arm_sqrt_q31\n");
    printf("%-9s %-9s %-9s\n", "Input", "Output", "Reference");
    for (size_t i = 0; i < n; i++) {
        printf("%-9d %-9d %-9d\n", in[i], out[i], ref[i]);
        ASSERT_NEAR(out[i], ref[i], epsilon) << "Error at index " << i;
    }
}

// 运行所有测试
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

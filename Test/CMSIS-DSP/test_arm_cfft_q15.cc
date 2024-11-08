#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "arm_math.h"
#include <gtest/gtest.h>

TEST(CMSISDSPTest, arm_cfft_q15)
{
#include "./data/fft_data_64_complex.c"
    arm_cfft_instance_q15 S;
    static q15_t outq15[FFT_LEN * 2];
    puts("\n\n=================================");
    printf("Q1.15实数FFT测试开始——arm_cfft_q15()\n");
    printf("FFT点数:  %d\n", FFT_LEN);

    // 拷贝数据到输出数组，arm_cfft_q15的输入输出是服用同一个数组的
    memcpy(outq15, FFT_IN, sizeof(FFT_IN));
    arm_cfft_init_q15(&S, FFT_LEN);
    arm_cfft_q15(&S, outq15, 0, 1);

    double rmse = 0;
    printf("%3s %6s %6s %s  %s %s %s\n", "i", "Q15-Real", "Q16-Imag", "Double-Real", "Double-Imag", "倍数-Real", "倍数-Imag");
    for (int i = 0; i < FFT_LEN; i++) {
        printf(
            "%4i %+6hd  %+6hd   %+.3e   %+.3e     %3.1f      %3.1f\n", //
            i,                                                         //
            outq15[i * 2], outq15[i * 2 + 1],                          //
            FFT_OUT[i * 2], FFT_OUT[i * 2 + 1],                        //
            FFT_OUT[i * 2] / outq15[i * 2],                            //
            FFT_OUT[i * 2 + 1] / outq15[i * 2 + 1]                     //
        );
        rmse += powl(FFT_OUT[i * 2] / FFT_LEN - outq15[i * 2], 2) + powl(FFT_OUT[i * 2 + 1] / FFT_LEN - outq15[i * 2 + 1], 2);
    }
    ASSERT_LT(rmse / FFT_LEN, 5);
    printf("均方根误差RMSE = %f\n", rmse / FFT_LEN);
    printf("\n");
    printf("Q1.15实数FFT测试结束\n=================================\n\n");
}

// 运行所有测试
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

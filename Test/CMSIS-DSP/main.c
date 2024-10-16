#include <stdio.h>

extern void test_arm_rfft_q15();

int main(int argc, char const *argv[])
{
    printf("CMSIS-DSP 测试开始\n");


    test_arm_rfft_q15();


    printf("CMSIS-DSP 测试结束\n");

    return 0;
}

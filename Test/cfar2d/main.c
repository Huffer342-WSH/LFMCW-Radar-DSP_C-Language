#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "radar_cfar.h"


void Test_upwrapper_speed(int N)
{
    volatile int x;
    clock_t start, end;
    double cpu_time_used0, cpu_time_used1;

    // 记录第一个循环的开始时间
    start = clock();
    for (int i = -N + 1; i < N; i++) {
        x = i & N;
    }
    // 记录第一个循环的结束时间
    end = clock();
    cpu_time_used0 = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("第一个循环耗时: %f 秒\n", cpu_time_used0);

    // 记录第二个循环的开始时间
    start = clock();
    for (int i = -N + 1; i < N; i++) {
        if (i < 0) {
            x = i + N;
        } else if (i >= N) {
            x = i - N;
        }
    }
    // 记录第二个循环的结束时间
    end = clock();
    cpu_time_used1 = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("第二个循环耗时: %f 秒\n", cpu_time_used1);
    if (cpu_time_used0 > cpu_time_used1) {
        printf("if判断速度更快 %f 秒\n", cpu_time_used0 - cpu_time_used1);
    } else {
        printf("与运算速度更快 %f 秒\n", cpu_time_used1 - cpu_time_used0);
    }
}

void Test_cfar2d()
{
    double arr[5][6] = { { 1, 2, 3, 20, 5, 6 },
                         { 13, 14, 15, 16, 17, 18 },
                         { 7, 8, 9, 10, 11, 12 },
                         { 19, 20, 21, 50, 23, 24 },
                         { 25, 90, 27, 28, 29, 30 } };
    cfar2d_result_t list;
    list.numMax = 20;
    list.point = (cfar2d_point_t *)malloc(sizeof(cfar2d_point_t) * list.numMax);

    radardsp_cfar2d(arr[0], &list, 5, 6, 1, 2, 0, 1, 1.1, 10);

    printf("\n\nCFAR-2D Result\n");
    for (int i = 0; i < list.numPoint; i++) {
        printf("point[%d]: %d, %d, %f, %f\n", i, list.point[i].idx0, list.point[i].idx1,
               list.point[i].amp, list.point[i].snr);
    }
#if 0
    int m = 10000;
    int n = 1 << 14;
    clock_t start, end;
    double cpu_time_used;
    double *a = (double *)malloc(m * n * sizeof(double));
    for (int i = 0; i < m * n; i++) {
        a[i] = i;
    }
    start = clock();
    radardsp_cfar2d(a, &list, m, n, 2, 10, 0, 1, 1.2, 2);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("CFAR-2D CPU耗时: %f 秒\n", cpu_time_used);
#endif
}
int main()
{
    printf("CFAR-2D Test\n");
    /*
    测试是if接缠绕和使用整除接缠绕的速度
    结果测试if的速度时更快的，可能和x86平台的乱序执行有关
    有待单片机平台进一步测试
    */
    // Test_upwrapper_speed(1 << 30);

    Test_cfar2d();



    return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ceigen.h"


// 示例程序
int test_inv()
{
    double original_matrix[4][4] = {
        { 7.00000000, 4.00000000, 8.00000000, 5.00000000 },
        { 7.00000000, 3.00000000, 7.00000000, 8.00000000 },
        { 5.00000000, 4.00000000, 8.00000000, 8.00000000 },
        { 3.00000000, 6.00000000, 5.00000000, 2.00000000 },
    };

    double ref_inverse_matrix[4][4] = {
        { 0.06374502, 0.33466135, -0.38645418, 0.04780876 },
        { -0.24701195, 0.20318725, -0.12749004, 0.31474104 },
        { 0.37450199, -0.53386454, 0.35458167, -0.21912351 },
        { -0.29083665, 0.22310757, 0.07569721, 0.03187251 },
    };

    int n = sizeof(original_matrix[0]) / sizeof(original_matrix[0][0]);


    double *inverse_matrix = (double *)malloc(n * n * sizeof(double));


    if (!matrix_inverse((double *)original_matrix, (double *)inverse_matrix, n)) {
        printf("矩阵的逆为：\n");
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                printf("%8.4f ", inverse_matrix[i * n + j]);
            }
            printf("\n");
        }
    } else {
        printf("矩阵不可逆。\n");
    }

    double sum = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            sum += fabs(inverse_matrix[i * n + j] - ref_inverse_matrix[i][j]);
        }
    }
    printf("误差:%f\n", sum);


    return 0;
}


void test_div()
{
    double A[4][3] = {
        { 81.0, 67.0, 25.0 },
        { 35.0, 24.0, 66.0 },
        { 85.0, 58.0, 54.0 },
        { 77.0, 79.0, 25.0 },
    };

    double B[3][3] = {
        { 6.0, 75.0, 86.0 },
        { 68.0, 74.0, 9.0 },
        { 64.0, 70.0, 11.0 },
    };

    double ref_C[4][3] = {
        { -0.40896013152486715, -17.051890669954805, 19.421598849157437 },
        { -0.400328812166051, -34.976674886970834, 37.747122893547086 },
        { -0.7151664611590638, -37.267879161529, 40.992293464858236 },
        { -0.1212494862309909, -8.211364570489122, 9.939066995478848 },
    };

    double C[4][3] = { 0 };

    matrix_div((double *)A, (double *)B, (double *)C, 4, 3);

    double sum = 0;
    printf("矩阵C为：\n");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%8.3lf ", C[i][j]);
        }
        printf("\r\n");
    }


    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            sum += fabs(ref_C[i][j] - C[i][j]);
        }
    }
    printf("误差:%f\n", sum);
}

int main(int argc, char const *argv[])
{
    test_inv();
    test_div();
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// 容忍误差
#define ERROR_TOLERANCE 1e-10

// 二维数组索引宏
#define INDEX(i, j) ((i) * (n) + (j))

// LU分解函数，返回1表示成功，0表示失败
int luDecomposition(double *A, double *L, double *U, int n)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (j < i) {
                L[INDEX(j, i)] = 0;
            } else {
                L[INDEX(j, i)] = A[INDEX(j, i)];
                for (int k = 0; k < i; k++) {
                    L[INDEX(j, i)] -= L[INDEX(j, k)] * U[INDEX(k, i)];
                }
            }
        }
        for (int j = 0; j < n; j++) {
            if (j < i) {
                U[INDEX(i, j)] = 0;
            } else if (fabs(L[INDEX(i, i)]) < ERROR_TOLERANCE) {
                return 0; // 奇异矩阵
            } else {
                U[INDEX(i, j)] = A[INDEX(i, j)] / L[INDEX(i, i)];
                for (int k = 0; k < i; k++) {
                    U[INDEX(i, j)] -= (L[INDEX(i, k)] * U[INDEX(k, j)]) / L[INDEX(i, i)];
                }
            }
        }
    }
    return 1;
}

// 前向替代法求解 L * y = b
void forwardSubstitution(double *L, double *b, double *y, int n)
{
    for (int i = 0; i < n; i++) {
        y[i] = b[i];
        for (int j = 0; j < i; j++) {
            y[i] -= L[INDEX(i, j)] * y[j];
        }
        y[i] /= L[INDEX(i, i)];
    }
}

// 后向替代法求解 U * x = y
void backwardSubstitution(double *U, double *y, double *x, int n)
{
    for (int i = n - 1; i >= 0; i--) {
        x[i] = y[i];
        for (int j = i + 1; j < n; j++) {
            x[i] -= U[INDEX(i, j)] * x[j];
        }
    }
}

// 矩阵求逆函数
int invertMatrix(double *A, double *A_inv, int n)
{
    // 创建L和U矩阵
    double *L = (double *)calloc(n * n, sizeof(double));
    double *U = (double *)calloc(n * n, sizeof(double));

    // LU分解
    if (!luDecomposition(A, L, U, n)) {
        free(L);
        free(U);
        return 0; // 矩阵不可逆
    }

    // 求逆矩阵列
    double *b = (double *)malloc(n * sizeof(double));
    double *y = (double *)malloc(n * sizeof(double));
    double *x = (double *)malloc(n * sizeof(double));

    for (int col = 0; col < n; col++) {
        // 单位向量b
        for (int i = 0; i < n; i++) {
            b[i] = (i == col) ? 1.0 : 0.0;
        }

        // 解 L * y = b
        forwardSubstitution(L, b, y, n);

        // 解 U * x = y
        backwardSubstitution(U, y, x, n);

        // 填充逆矩阵列
        for (int i = 0; i < n; i++) {
            A_inv[INDEX(i, col)] = x[i];
        }
    }

    // 释放内存
    free(L);
    free(U);
    free(b);
    free(y);
    free(x);

    return 1;
}

// 示例程序
int main()
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


    if (invertMatrix((double *)original_matrix, (double *)inverse_matrix, n)) {
        printf("矩阵的逆为：\n");
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                printf("%8.4f ", inverse_matrix[INDEX(i, j)]);
            }
            printf("\n");
        }
    } else {
        printf("矩阵不可逆。\n");
    }

    double sum = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            sum += fabs(inverse_matrix[INDEX(i, j)] - ref_inverse_matrix[i][j]);
        }
    }
    printf("误差:%f\n", sum);


    return 0;
}

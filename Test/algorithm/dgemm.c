#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void dgemm(const int order, const char transa, const char transb, const int m, const int n, const int k, const double alpha, const double *A, const int lda,
           const double *B, const int ldb, const double beta, double *C, const int ldc)
{
    // Check if the input matrices are valid
    if (transa != 'N' && transa != 'T') {
        fprintf(stderr, "Invalid value for transa. Expected 'N' or 'T'.\n");
        return;
    }
    if (transb != 'N' && transb != 'T') {
        fprintf(stderr, "Invalid value for transb. Expected 'N' or 'T'.\n");
        return;
    }

    // Handle transposition of A and B
    int m1 = (transa == 'N') ? m : k;
    int k1 = (transa == 'N') ? k : m;
    int n1 = (transb == 'N') ? n : k;

    if (order) {
#define AN(i, j) A[i * lda + j]
#define BN(i, j) B[i * ldb + j]
#define AT(i, j) A[j * lda + i]
#define BT(i, j) B[j * ldb + i]
#define CN(i, j) C[i * ldc + j]
        if (transa == 'N') {
            if (transb == 'N') {
                for (int i = 0; i < m; ++i) {
                    for (int j = 0; j < n; ++j) {
                        CN(i, j) *= beta;
                        for (int l = 0; l < k; ++l) {
                            CN(i, j) += alpha * AN(i, l) * BN(l, j);
                        }
                    }
                }
            } else {
                for (int i = 0; i < m; ++i) {
                    for (int j = 0; j < n; ++j) {
                        CN(i, j) *= beta;
                        for (int l = 0; l < k; ++l) {
                            CN(i, j) += alpha * AN(i, l) * BT(l, j);
                        }
                    }
                }
            }
        } else {
            if (transb == 'N') {
                for (int i = 0; i < m; ++i) {
                    for (int j = 0; j < n; ++j) {
                        CN(i, j) *= beta;
                        for (int l = 0; l < k; ++l) {
                            CN(i, j) += alpha * AT(i, l) * BN(l, j);
                        }
                    }
                }

            } else {
                for (int i = 0; i < m; ++i) {
                    for (int j = 0; j < n; ++j) {
                        CN(i, j) *= beta;
                        for (int l = 0; l < k; ++l) {
                            CN(i, j) += alpha * AT(i, l) * BT(l, j);
                        }
                    }
                }
            }
        }
#undef AN
#undef BN
#undef AT
#undef BT
#undef CN
    } else {
    }
}


void print_matrix(double *matrix, int rows, int cols)
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%lf ", matrix[i * cols + j]);
        }
        printf("\n");
    }
    printf("\n");
}
int main()
{
    // Example usage
    double A[6] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0 };    // 2x3 matrix
    double B[6] = { 7.0, 8.0, 9.0, 10.0, 11.0, 12.0 }; // 3x2 matrix
    double C[4] = { 0.0, 0.0, 0.0, 0.0 };              // 2x2 matrix for output
    double alpha = 1.0;
    double beta = 0.0;

    // Flag to choose row-major matrix multiplication
    int use_row_major = 1; // Set to 1 for row-major, 0 for column-major

    // Call the matrix multiplication function with row-major


    dgemm(use_row_major, 'N', 'N', 2, 2, 3, alpha, A, 3, B, 2, beta, C, 2);
    print_matrix(C, 2, 2);

    dgemm(use_row_major, 'N', 'T', 2, 2, 3, alpha, A, 3, B, 3, beta, C, 2);
    print_matrix(C, 2, 2);

    dgemm(use_row_major, 'T', 'N', 2, 2, 3, alpha, A, 2, B, 2, beta, C, 2);
    print_matrix(C, 2, 2);

    dgemm(use_row_major, 'T', 'T', 2, 2, 3, alpha, A, 2, B, 3, beta, C, 2);
    print_matrix(C, 2, 2);


    return 0;
}

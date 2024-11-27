#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int matrix_inverse(double *A, double *A_inv, int n);
int matrix_div(double *A, double *B, double *C, int m, int n);
#ifdef __cplusplus
}
#endif

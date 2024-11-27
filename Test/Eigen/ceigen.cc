#include <Eigen/Dense>
#include <iostream>
#ifdef __cplusplus
extern "C" {
#endif
int matrix_inverse(double *A, double *A_inv, int n)
{
    // 使用Eigen的Map类直接映射C风格数组为Eigen矩阵
    Eigen::Map<Eigen::MatrixXd> matA(A, n, n);

    // 使用Eigen的Map类映射输出数组A_inv为一个矩阵
    Eigen::Map<Eigen::MatrixXd> matA_inv(A_inv, n, n);

    // 计算逆矩阵并直接存储到A_inv
    matA_inv = matA.inverse();
    return 0;
}

/**
 * @brief C = A/B <==> C = A*inv(B)
 *
 * @param A mxn
 * @param B nxn
 * @param C mxn
 * @param n
 * @return int
 */
int matrix_div(double *A, double *B, double *C, int m, int n)
{
    // 使用Eigen的Map类直接映射C风格数组为Eigen矩阵
    Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> matA(A, m, n);

    // 使用Eigen的Map类映射输出数组A_inv为一个矩阵
    Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> matB(B, n, n);

    Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> matC(C, m, n);


    std::cout << matA << std::endl;

    // 计算逆矩阵并直接存储到A_inv
    matC = matB.transpose().fullPivLu().solve(matA.transpose()).transpose();
    return 0;
}


#ifdef __cplusplus
}
#endif

#include <Eigen/Dense>
#include <iostream>
#include <cstdio>
#include <cstdlib>

using namespace std;

int test_matrix_size()
{
    cout << "Size of Eigen::Matrix<double, 1, 2> is " << sizeof(Eigen::Matrix<double, 1, 2>) << endl;
    cout << "Size of Eigen::Matrix<double, 2, 2> is " << sizeof(Eigen::Matrix<double, 2, 2>) << endl;
    cout << "Size of Eigen::Matrix<double, 3, 2> is " << sizeof(Eigen::Matrix<double, 3, 2>) << endl;
    cout << "Size of Eigen::Matrix<double, 3, 2> is " << sizeof(Eigen::Matrix<double, 3, 2>) << endl;

    return 0;
}

int main(int argc, char const *argv[])
{
    test_matrix_size();
    return 0;
}

#include <Eigen/Dense>
#include <iostream>
#include <cstdio>
#include <cstdlib>


Eigen::MatrixXd fun()
{
    Eigen::MatrixXd ret(3, 3);
    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++) {
            ret(i, j) = i * 3 + j;
        }
    }
    printf("Matrix Address in function: %p\n", &ret);
    printf("Data Address in function: %p\n", ret.data());

    return ret;
}

Eigen::Matrix3d fun_Matrix3d()
{
    Eigen::Matrix3d ret(3, 3);
    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++) {
            ret(i, j) = i * 3 + j;
        }
    }
    printf("Matrix Address in function: %p\n", &ret);
    printf("Data Address in function: %p\n", ret.data());

    return ret;
}

int test_function_return()
{
    Eigen::MatrixXd x;
    x = fun();
    std::cout << x << std::endl;
    for (size_t i = 0; i < x.size(); i++) {
        printf("%f ", x.data()[i]);
    }
    printf("Matrix Address outside: %p\n", &x);
    printf("Data Address outside: %p\n", x.data());

    x << 1, 1, 1, 1, 1, 1, 1, 1, 1;
    std::cout << x << std::endl;
    printf("Matrix Address outside: %p\n", &x);
    printf("Data Address outside: %p\n", x.data());


    Eigen::MatrixXd y;
    y = fun_Matrix3d();
    std::cout << y << std::endl;
    printf("Matrix Address outside: %p\n", &y);
    printf("Data Address outside: %p\n", y.data());


    return 0;
}

int test_assignment_add()
{
    Eigen::MatrixXd a(3, 3), b(3, 3), c;
    a << 1, 1, 1, 1, 1, 1, 1, 1, 1;
    b << 1, 1, 1, 1, 1, 1, 1, 1, 1;
    c = a + b;

    printf("Data addr a: %p\n", a.data());
    printf("Data addr b: %p\n", b.data());
    printf("Data addr c: %p\n", c.data());
    return 0;
}

int test_assignment_init()
{
    static double a[2][3] = { { 1, 2, 3 }, { 4, 5, 6 } };

    // 使用 Eigen::Map 将数组 a 映射为 Eigen 矩阵
    Eigen::Matrix<double, 2, 3> mat = Eigen::Map<Eigen::Matrix<double, 2, 3, Eigen::RowMajor>>(&a[0][0]);

    // 输出矩阵
    std::cout << "Eigen matrix:\n" << mat << std::endl;
    return 0;
}

int test_assignment_typeconvert()
{
    // 定义一个 float 类型的矩阵
    Eigen::Matrix<float, 2, 3> matFloat;
    matFloat << 1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f;

    // 定义一个 double 类型的矩阵
    Eigen::Matrix<double, 2, 3> matDouble;

    // 自动类型转换赋值
    matDouble = matFloat.cast<double>();

    // 输出两个矩阵
    std::cout << "Float matrix:\n" << matFloat << std::endl;
    std::cout << "Double matrix (after assignment):\n" << matDouble << std::endl;

    return 0;
}
int main(int argc, char const *argv[])
{
    void *p = malloc(4);
    printf("Stack Address: %p\n", &p);
    printf("Heap Address: %p\n", p);
    free(p);

    test_assignment_typeconvert();
    test_assignment_init();
    test_function_return();
    test_assignment_add();
    return 0;
}

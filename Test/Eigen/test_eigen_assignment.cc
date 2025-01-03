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

int main(int argc, char const *argv[])
{
    void *p = malloc(4);
    printf("Stack Address: %p\n", &p);
    printf("Heap Address: %p\n", p);
    free(p);

    test_function_return();
    test_assignment_add();
    return 0;
}

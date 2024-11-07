#include "radar_math_types.h"
#include "radar_error.h"

#include <stdlib.h>
/**
 * @brief 将一块内存看作一个2D整数矩阵
 * @param m  matrix2d_int16_t 结构体，用于保存2D整数矩阵的信息
 * @param buffer  保存2D整数矩阵的数据的指针
 * @param n0  2D整数矩阵的0维度的大小
 * @param n1  2D整数矩阵的1维度的大小
 * @param owner  该函数是否拥有这个内存块的所有权
 * @return 0 - success
 * @details 该函数将buffer指向的数据看作一个2D整数矩阵，并将其信息保存到m中
 */
int radar_matrix2d_int16_setData(matrix2d_complex_int16_t *m, int16_t *data, size_t n0, size_t n1, int owner)
{
    m->size0 = n0;
    m->size1 = n1;
    m->tda = n1;
    m->data = data;
    m->owner = owner;
    return 0;
}


matrix2d_complex_int16_t *radar_matrix2d_complex_int16_alloc(size_t n0, size_t n1)
{
    matrix2d_complex_int16_t *m;
    m = (matrix2d_complex_int16_t *)malloc(sizeof(matrix2d_complex_int16_t));

    if (m == NULL) {
        RADAR_ERROR("failed to allocate space for matrix struct", RADAR_ENOMEM);
    }

    m->data = (int16_t *)malloc(sizeof(int16_t) * n0 * n1 * 2);

    if (m->data == NULL) {
        RADAR_ERROR("failed to allocate space for matrix data", RADAR_ENOMEM);
    }

    m->size0 = n0;
    m->size1 = n1;
    m->tda = n1;
    m->owner = 1;
    return m;
}


matrix2d_int16_t *radar_matrix2d_int16_alloc(size_t n0, size_t n1)
{
    matrix2d_int16_t *m;
    m = (matrix2d_int16_t *)malloc(sizeof(matrix2d_int16_t));

    if (m == NULL) {
        RADAR_ERROR("failed to allocate space for matrix struct", RADAR_ENOMEM);
    }

    m->data = (int16_t *)malloc(sizeof(int16_t) * n0 * n1);

    if (m->data == NULL) {
        RADAR_ERROR("failed to allocate space for matrix data", RADAR_ENOMEM);
    }

    m->size0 = n0;
    m->size1 = n1;
    m->tda = n1;
    m->owner = 1;
    return m;
}

matrix2d_int32_t *radar_matrix2d_int32_alloc(size_t n0, size_t n1)
{
    matrix2d_int32_t *m;
    m = (matrix2d_int32_t *)malloc(sizeof(matrix2d_int32_t));

    if (m == NULL) {
        RADAR_ERROR("failed to allocate space for matrix struct", RADAR_ENOMEM);
    }

    m->data = (int16_t *)malloc(sizeof(int16_t) * n0 * n1);

    if (m->data == NULL) {
        RADAR_ERROR("failed to allocate space for matrix data", RADAR_ENOMEM);
    }

    m->size0 = n0;
    m->size1 = n1;
    m->tda = n1;
    m->owner = 1;
    return m;
}

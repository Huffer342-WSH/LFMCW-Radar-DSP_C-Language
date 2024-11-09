#include "radar_matrix.h"

#include "radar_error.h"
#include "radar_assert.h"


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/**
 * @brief 返回一个大于等于ptr且是alignment的倍数的指针
 *
 * @param ptr
 * @param alignment
 * @return void*
 */
static inline void *align_ptr(void *ptr, size_t alignment)
{
    uintptr_t p = (uintptr_t)ptr;
    uintptr_t aligned = (p + alignment - 1) & ~(alignment - 1);
    return (void *)aligned;
}


// MARK: alloc


/**
 * @brief 在堆上分配一个matrix3d_complex_int16_t矩阵
 *
 * @param n0 矩阵的第一个维度的大小
 * @param n1 矩阵的第二个维度的大小
 * @param n2 矩阵的第三个维度的大小
 * @return 成功时返回指向分配的matrix3d_complex_int16_t结构的指针;
 *         如果无法分配，返回NULL
 *
 * @note
 *   该函数在堆上分配matrix3d_complex_int16_t矩阵。如果
 *   分配失败，RADAR_ERROR()函数将被调用，
 *   且返回NULL
 */
matrix3d_complex_int16_t *radar_matrix3d_complex_int16_alloc(size_t n0, size_t n1, size_t n2)
{
    matrix3d_complex_int16_t *m;
    m = (matrix3d_complex_int16_t *)malloc(sizeof(matrix3d_complex_int16_t));
    if (m == NULL) {
        RADAR_ERROR("failed to allocate space for matrix struct", RADAR_ENOMEM);
    }

    m->data = (int16_t *)malloc(sizeof(int16_t) * n0 * n1 * n2 * 2);
    if (m->data == NULL) {
        RADAR_ERROR("failed to allocate space for matrix data", RADAR_ENOMEM);
    }

    m->size0 = n0;
    m->size1 = n1;
    m->size2 = n2;
    m->tda1 = n1 * n2;
    m->tda2 = n2;
    m->owner = 1;
    return m;
}


/**
 * @brief 在堆上分配一个matrix2d_complex_int32_t矩阵
 *
 * @param n0 矩阵的第一个维度的大小
 * @param n1 矩阵的第二个维度的大小
 * @return 成功时返回指向分配的matrix2d_complex_int32_t结构的指针;
 *         如果无法分配，返回NULL
 *
 * @note
 *   该函数在堆上分配matrix2d_complex_int32_t矩阵。如果
 *   分配失败，RADAR_ERROR()函数将被调用，
 *   且返回NULL
 */
matrix2d_complex_int32_t *radar_matrix2d_complex_int32_alloc(size_t n0, size_t n1)
{
    matrix2d_complex_int32_t *m;
    m = (matrix2d_complex_int32_t *)malloc(sizeof(matrix2d_complex_int32_t));
    if (m == NULL) {
        RADAR_ERROR("failed to allocate space for matrix struct", RADAR_ENOMEM);
    }

    m->data = (int32_t *)malloc(sizeof(int32_t) * n0 * n1 * 2);
    if (m->data == NULL) {
        RADAR_ERROR("failed to allocate space for matrix data", RADAR_ENOMEM);
    }

    m->size0 = n0;
    m->size1 = n1;
    m->tda1 = n1;
    m->owner = 1;
    return m;
}


/**
 * @brief 在堆上分配一个matrix2d_complex_int16_t矩阵
 *
 * @param n0 矩阵的第一个维度的大小
 * @param n1 矩阵的第二个维度的大小
 * @return 成功时返回指向分配的matrix2d_complex_int16_t结构的指针;
 *         如果无法分配，返回NULL
 *
 * @note
 *   该函数在堆上分配matrix2d_complex_int16_t矩阵。如果
 *   分配失败，RADAR_ERROR()函数将被调用，
 *   且返回NULL
 */
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
    m->tda1 = n1;
    m->owner = 1;
    return m;
}


/**
 * @brief 在堆上分配一个matrix2d_int32_t矩阵
 *
 * @param n0 矩阵的第一个维度的大小
 * @param n1 矩阵的第二个维度的大小
 * @return 成功时返回指向分配的matrix2d_int32_t结构的指针;
 *         如果无法分配，返回NULL
 *
 * @note
 *   该函数在堆上分配matrix2d_int32_t矩阵。如果
 *   分配失败，RADAR_ERROR()函数将被调用,
 *   且返回NULL
 */
matrix2d_int32_t *radar_matrix2d_int32_alloc(size_t n0, size_t n1)
{
    matrix2d_int32_t *m;
    m = (matrix2d_int32_t *)malloc(sizeof(matrix2d_int32_t));

    if (m == NULL) {
        RADAR_ERROR("failed to allocate space for matrix struct", RADAR_ENOMEM);
    }

    m->data = (int32_t *)malloc(sizeof(int32_t) * n0 * n1);

    if (m->data == NULL) {
        RADAR_ERROR("failed to allocate space for matrix data", RADAR_ENOMEM);
    }

    m->size0 = n0;
    m->size1 = n1;
    m->tda1 = n1;
    m->owner = 1;
    return m;
}


/**
 * @brief 在堆上分配一个matrix2d_int16_t矩阵
 *
 * @param n0 矩阵的第一个维度的大小
 * @param n1 矩阵的第二个维度的大小
 * @return 成功时返回指向分配的matrix2d_int16_t结构的指针;
 *         如果无法分配，返回NULL
 *
 * @note
 *   该函数在堆上分配matrix2d_int16_t矩阵。如果
 *   分配失败，RADAR_ERROR()函数将被调用,
 *   且返回NULL
 */
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
    m->tda1 = n1;
    m->owner = 1;
    return m;
}

// MARK: alloc static

/**
 * @brief 从提供的缓冲区中分配一个matrix3d_complex_int16_t矩阵
 *
 * @param buffer 指向用来存储矩阵数据的缓冲区。希望缓冲区起始地址符合matrix3d_complex_int16_t的对齐要求，以节省空间。
 * @param size 缓冲区的大小（以字节为单位）。
 * @param n0 矩阵的第一个维度的大小。
 * @param n1 矩阵的第二个维度的大小。
 * @param n2 矩阵的第三个维度的大小。
 * @return 成功时返回指向分配的matrix3d_complex_int16_t结构的指针；
 *         如果缓冲区大小不足，返回NULL。
 *
 * @note 该函数用于从静态缓冲区中分配matrix3d_complex_int16_t矩阵，动态分配请使用radar_matrix3d_complex_int16_alloc()函数。
 */
matrix3d_complex_int16_t *radar_matrix3d_complex_int16_alloc_static(void *buffer, size_t size, size_t n0, size_t n1, size_t n2)
{
    matrix3d_complex_int16_t *m;
    int16_t *pData;

    size_t data_size = 2 * n0 * n1 * n2 * sizeof(int16_t);

    m = (matrix3d_complex_int16_t *)align_ptr(buffer, alignof(matrix3d_complex_int16_t));
    pData = (int16_t *)align_ptr((void *)m + sizeof(matrix3d_complex_int16_t), alignof(int16_t));

    // 检查缓冲区是否足够大
    if ((uintptr_t)pData + data_size > (uintptr_t)buffer + size) {
        return NULL; // 缓冲区不足
    }

    m->data = pData;
    m->size0 = n0;
    m->size1 = n1;
    m->size2 = n2;
    m->tda1 = n1 * n2;
    m->tda2 = n2;
    m->owner = -1;
    return m;
}


/**
 * @brief 从提供的缓冲区中分配一个matrix2d_complex_int32_t矩阵
 *
 * @param buffer 指向用来存储矩阵数据的缓冲区。希望缓冲区起始地址符合matrix2d_complex_int32_t的对齐要求，以节省空间。
 * @param size 缓冲区的大小（以字节为单位）。
 * @param n0 矩阵的第一个维度的大小。
 * @param n1 矩阵的第二个维度的大小。
 * @return 成功时返回指向分配的matrix2d_complex_int32_t结构的指针;
 *         如果缓冲区大小不足，返回NULL。
 *
 * @note 该函数用于从静态缓冲区中分配matrix2d_complex_int32_t矩阵，动态分配请使用radar_matrix2d_complex_int32_alloc()函数。
 */
matrix2d_complex_int32_t *radar_matrix2d_complex_int32_alloc_static(void *buffer, size_t size, size_t n0, size_t n1)
{
    matrix2d_complex_int32_t *m;
    int32_t *pData;

    size_t data_size = 2 * n0 * n1 * sizeof(int32_t); // 复数数据，占两个int32_t（实部和虚部）

    m = (matrix2d_complex_int32_t *)align_ptr(buffer, alignof(matrix2d_complex_int32_t));
    pData = (int32_t *)align_ptr((void *)m + sizeof(matrix2d_complex_int32_t), alignof(int32_t));

    // 检查缓冲区是否足够大
    if ((uintptr_t)pData + data_size > (uintptr_t)buffer + size) {
        return NULL; // 缓冲区不足
    }

    m->data = pData;
    m->size0 = n0;
    m->size1 = n1;
    m->tda1 = n1;  // tda1 是行步长，通常等于列数
    m->owner = -1; // 所有者标志
    return m;
}


/**
 * @brief 从提供的缓冲区中分配一个matrix2d_complex_int16_t矩阵
 *
 * @param buffer 指向用来存储矩阵数据的缓冲区。希望缓冲区起始地址符合matrix2d_complex_int16_t的对齐要求，以节省空间。
 * @param size 缓冲区的大小（以字节为单位）。
 * @param n0 矩阵的第一个维度的大小。
 * @param n1 矩阵的第二个维度的大小。
 * @return 成功时返回指向分配的matrix2d_complex_int16_t结构的指针;
 *         如果缓冲区大小不足，返回NULL。
 *
 * @note 该函数用于从静态缓冲区中分配matrix2d_complex_int16_t矩阵，动态分配请使用radar_matrix2d_complex_int16_alloc()函数。
 */
matrix2d_complex_int16_t *radar_matrix2d_complex_int16_alloc_static(void *buffer, size_t size, size_t n0, size_t n1)
{
    matrix2d_complex_int16_t *m;
    int16_t *pData;

    size_t data_size = 2 * n0 * n1 * sizeof(int16_t); // 复数数据，占两个int16_t（实部和虚部）

    m = (matrix2d_complex_int16_t *)align_ptr(buffer, alignof(matrix2d_complex_int16_t));
    pData = (int16_t *)align_ptr((void *)m + sizeof(matrix2d_complex_int16_t), alignof(int16_t));

    // 检查缓冲区是否足够大
    if ((uintptr_t)pData + data_size > (uintptr_t)buffer + size) {
        return NULL; // 缓冲区不足
    }

    m->data = pData;
    m->size0 = n0;
    m->size1 = n1;
    m->tda1 = n1;  // tda1 是行步长，通常等于列数
    m->owner = -1; // 所有者标志
    return m;
}


matrix2d_int32_t *radar_matrix2d_int32_alloc_static(void *buffer, size_t size, size_t n0, size_t n1)
{
    matrix2d_int32_t *m;
    int32_t *pData;

    size_t data_size = n0 * n1 * sizeof(int32_t); // 实数数据，占一个int32_t

    m = (matrix2d_int32_t *)align_ptr(buffer, alignof(matrix2d_int32_t));
    pData = (int32_t *)align_ptr((void *)m + sizeof(matrix2d_int32_t), alignof(int32_t));

    // 检查缓冲区是否足够大
    if ((uintptr_t)pData + data_size > (uintptr_t)buffer + size) {
        return NULL; // 缓冲区不足
    }

    m->data = pData;
    m->size0 = n0;
    m->size1 = n1;
    m->tda1 = n1;  // tda1 是行步长，通常等于列数
    m->owner = -1; // 所有者标志
    return m;
}


matrix2d_int16_t *radar_matrix2d_int16_alloc_static(void *buffer, size_t size, size_t n0, size_t n1)
{
    matrix2d_int16_t *m;
    int16_t *pData;

    size_t data_size = n0 * n1 * sizeof(int16_t); // 实数数据，占一个int16_t

    m = (matrix2d_int16_t *)align_ptr(buffer, alignof(matrix2d_int16_t));
    pData = (int16_t *)align_ptr((void *)m + sizeof(matrix2d_int16_t), alignof(int16_t));

    // 检查缓冲区是否足够大
    if ((uintptr_t)pData + data_size > (uintptr_t)buffer + size) {
        return NULL; // 缓冲区不足
    }

    m->data = pData;
    m->size0 = n0;
    m->size1 = n1;
    m->tda1 = n1;  // tda1 是行步长，通常等于列数
    m->owner = -1; // 所有者标志
    return m;
}


// MARK: set_data

int radar_matrix3d_complex_int16_set_data(matrix3d_complex_int16_t *m, int16_t *data, size_t size, size_t n0, size_t n1, size_t n2, int owner)
{
    RADAR_ASSERT(size == n0 * n1 * n2 * 2);
    if (m->owner) {
        free(m->data);
    }
    m->size0 = n0;
    m->size1 = n1;
    m->size2 = n2;
    m->tda1 = n1 * n2;
    m->tda2 = n2;
    m->data = data;
    m->owner = owner;
    return 0;
}

int radar_matrix2d_complex_int32_set_data(matrix2d_complex_int32_t *m, int32_t *data, size_t size, size_t n0, size_t n1, int owner)
{
    RADAR_ASSERT(size == n0 * n1 * 2);
    if (m->owner) {
        free(m->data);
    }
    m->size0 = n0;
    m->size1 = n1;
    m->tda1 = n1;
    m->data = data;
    m->owner = owner;
    return 0;
}

int radar_matrix2d_complex_int16_set_data(matrix2d_complex_int16_t *m, int16_t *data, size_t size, size_t n0, size_t n1, int owner)
{
    RADAR_ASSERT(size == n0 * n1 * 2);
    if (m->owner) {
        free(m->data);
    }
    m->size0 = n0;
    m->size1 = n1;
    m->tda1 = n1;
    m->data = data;
    m->owner = owner;
    return 0;
}

int radar_matrix2d_int32_set_data(matrix2d_int32_t *m, int32_t *data, size_t size, size_t n0, size_t n1, int owner)
{
    RADAR_ASSERT(size == n0 * n1);
    if (m->owner) {
        free(m->data);
    }
    m->size0 = n0;
    m->size1 = n1;
    m->tda1 = n1;
    m->data = data;
    m->owner = owner;
    return 0;
}

int radar_matrix2d_int16_set_data(matrix2d_int16_t *m, int16_t *data, size_t size, size_t n0, size_t n1, int owner)
{
    RADAR_ASSERT(size == n0 * n1);
    if (m->owner) {
        free(m->data);
    }
    m->size0 = n0;
    m->size1 = n1;
    m->tda1 = n1;
    m->data = data;
    m->owner = owner;
    return 0;
}

// MARK: free


void radar_matrix3d_complex_int16_free(matrix3d_complex_int16_t *m)
{
    if (m->owner) {
        free(m->data);
    }
    if (m->owner != -1) {
        free(m);
    }
}

void radar_matrix2d_complex_int32_free(matrix2d_complex_int32_t *m)
{
    if (m->owner) {
        free(m->data);
    }
    if (m->owner != -1) {
        free(m->data);
    }
}

void radar_matrix2d_complex_int16_free(matrix2d_complex_int16_t *m)
{
    if (m->owner) {
        free(m->data);
    }
    if (m->owner != -1) {
        free(m);
    }
}

void radar_matrix2d_int32_free(matrix2d_int32_t *m)
{
    if (m->owner) {
        free(m->data);
    }
    if (m->owner != -1) {
        free(m);
    }
}

void radar_matrix2d_int16_free(matrix2d_int16_t *m)
{
    if (m->owner) {
        free(m->data);
    }
    if (m->owner != -1) {
        free(m);
    }
}

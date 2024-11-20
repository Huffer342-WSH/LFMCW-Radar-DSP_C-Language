/**
 * @file radar_matrix.h
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 定义矩阵
 * @version 0.1
 * @date 2024-11-09
 *
 * @note owner == 1 代表data由matrix管理，会在matrix销毁时自动释放； owner == -1 表示matrix自身也不会free
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdalign.h>


#ifdef __cplusplus
extern "C" {
#endif

/** @brief 返回一个大于等于ptr且是alignment的倍数的指针 */
#define ALIGN_ADDRESS(ptr, alignment) (((uintptr_t)(ptr) + alignment - 1) & (~((uintptr_t)alignment - 1)))


/**
 * @brief 返回矩阵在静态分配时，内存需要对齐的字节数
 *
 */
#define MATRIX_STATIC_BUFFER_ALIGN(mat_type, elm_type) (alignof(mat_type) > alignof(elm_type) ? alignof(mat_type) : alignof(elm_type))


/**
 * @brief 返回静态分配矩阵需要的内存大小（矩阵需要对齐）
 * @param mat_type 矩阵类型，如matrix3d_complex_int16
 * @param elm_type 矩阵中元素的类型，如int16
 * @param elm_num  矩阵中元素的个数
 */
#define MATRIX_STATIC_BUFFER_SIZE(mat_type, elm_type, elm_num) (ALIGN_ADDRESS(sizeof(mat_type), alignof(elm_type)) + elm_num * sizeof(elm_type))


typedef struct matrix3d_complex_int16 {
    size_t size0;
    size_t size1;
    size_t size2;
    size_t tda1;
    size_t tda2;
    int16_t *data;
    int owner;
} matrix3d_complex_int16_t;

typedef struct {
    size_t size0;
    size_t size1;
    size_t tda1;
    int32_t *data;
    int owner;
} matrix2d_complex_int32_t;


typedef struct {
    size_t size0;
    size_t size1;
    size_t tda1;
    int16_t *data;
    int owner;
} matrix2d_complex_int16_t;


typedef struct {
    size_t size0;
    size_t size1;
    size_t tda1;
    int32_t *data;
    int owner;
} matrix2d_int32_t;

typedef struct {
    size_t size0;
    size_t size1;
    size_t tda1;
    int16_t *data;
    int owner;
} matrix2d_int16_t;


matrix3d_complex_int16_t *radar_matrix3d_complex_int16_alloc(size_t n0, size_t n1, size_t n2);
matrix2d_complex_int32_t *radar_matrix2d_complex_int32_alloc(size_t n0, size_t n1);
matrix2d_complex_int16_t *radar_matrix2d_complex_int16_alloc(size_t n0, size_t n1);
matrix2d_int32_t *radar_matrix2d_int32_alloc(size_t n0, size_t n1);
matrix2d_int16_t *radar_matrix2d_int16_alloc(size_t n0, size_t n1);


/** @example 从静态内存分配矩阵例子

    static __attribute__((aligned(MATRIX_STATIC_BUFFER_ALIGN(
        matrix3d_complex_int16_t, int16_t)))) uint8_t buffer[MATRIX_STATIC_BUFFER_SIZE(matrix3d_complex_int16_t, int16_t, 10 * 20 * 30 * 2)];
    matrix3d_complex_int16_t *m = radar_matrix3d_complex_int16_alloc_static(buffer, sizeof(buffer), 10, 20, 30);
 */
matrix3d_complex_int16_t *radar_matrix3d_complex_int16_alloc_static(void *buffer, size_t size, size_t n0, size_t n1, size_t n2);
matrix2d_complex_int32_t *radar_matrix2d_complex_int32_alloc_static(void *buffer, size_t size, size_t n0, size_t n1);
matrix2d_complex_int16_t *radar_matrix2d_complex_int16_alloc_static(void *buffer, size_t size, size_t n0, size_t n1);
matrix2d_int32_t *radar_matrix2d_int32_alloc_static(void *buffer, size_t size, size_t n0, size_t n1);
matrix2d_int16_t *radar_matrix2d_int16_alloc_static(void *buffer, size_t size, size_t n0, size_t n1);


int radar_matrix3d_complex_int16_set_data(matrix3d_complex_int16_t *m, int16_t *data, size_t size, size_t n0, size_t n1, size_t n2, int owner);
int radar_matrix2d_complex_int32_set_data(matrix2d_complex_int32_t *m, int32_t *data, size_t size, size_t n0, size_t n1, int owner);
int radar_matrix2d_complex_int16_set_data(matrix2d_complex_int16_t *m, int16_t *data, size_t size, size_t n0, size_t n1, int owner);
int radar_matrix2d_int32_set_data(matrix2d_int32_t *m, int32_t *data, size_t size, size_t n0, size_t n1, int owner);
int radar_matrix2d_int16_set_data(matrix2d_int16_t *m, int16_t *data, size_t size, size_t n0, size_t n1, int owner);


void radar_matrix3d_complex_int16_free(matrix3d_complex_int16_t *m);
void radar_matrix2d_complex_int32_free(matrix2d_complex_int32_t *m);
void radar_matrix2d_complex_int16_free(matrix2d_complex_int16_t *m);
void radar_matrix2d_int32_free(matrix2d_int32_t *m);
void radar_matrix2d_int16_free(matrix2d_int16_t *m);

#ifdef __cplusplus
}
#endif

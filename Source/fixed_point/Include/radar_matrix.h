#pragma once

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
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

int radar_matrix3d_complex_int16_setData(matrix3d_complex_int16_t *m, int16_t *data, size_t size, size_t n0, size_t n1, size_t n2, int owner);
int radar_matrix2d_int16_setData(matrix2d_complex_int16_t *m, int16_t *data, size_t n0, size_t n1, int owner);


#ifdef __cplusplus
}
#endif

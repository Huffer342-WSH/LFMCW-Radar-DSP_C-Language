#ifndef _RADAR_MATRIX_H_
#define _RADAR_MATRIX_H_

#include <stdint.h>

typedef struct {
    size_t size0;
    size_t size1;
    size_t tda;
    int16_t *data;
    int owner;
} matrix2d_complex_int16_t;

typedef struct {
    size_t size0;
    size_t size1;
    size_t tda;
    int16_t *data;
    int owner;
} matrix2d_int16_t;


typedef struct {
    size_t size0;
    size_t size1;
    size_t tda;
    int32_t *data;
    int owner;
} matrix2d_int32_t;

int radar_matrix2d_int16_setData(matrix2d_complex_int16_t *m, int16_t *data, size_t n0, size_t n1, int owner);

matrix2d_complex_int16_t *radar_matrix2d_complex_int16_alloc(size_t n0, size_t n1);

matrix2d_int16_t *radar_matrix2d_int16_alloc(size_t n0, size_t n1);

matrix2d_int32_t *radar_matrix2d_int32_alloc(size_t n0, size_t n1);


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
#endif /* _RADAR_MATRIX_H_ */

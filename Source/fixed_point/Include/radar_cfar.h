#ifndef _RADAR_CFAR_H_
#define _RADAR_CFAR_H_

#include "radar_math_types.h"

typedef struct {
    uint16_t idx0;
    uint16_t idx1;
    int32_t amp;
    int32_t snr; // 8位小数部分
} cfar2d_point_t;

/**
 * @brief 2D-CFAR检测结果
 */
typedef struct {
    cfar2d_point_t *point; /* 需要外部分配内存然后赋值 */
    size_t capacity;
    size_t numPoint;
    uint8_t is_point_need_free  : 1;
    uint8_t is_struct_need_free : 1;
    uint8_t reserved            : 6;
} cfar2d_result_t;

/**
 * @brief CFAR配置
 *
 */
typedef struct {
    uint16_t numTrain[2];
    uint16_t numGuard[2];
    float thSNR;
    int32_t thAmp; // 不同距离单元的幅度阈值
} cfar2d_cfg_t;

/**
 * @brief CFAR结果删点的配置
 *
 */
typedef struct {
    uint16_t range0;
    uint16_t range1;
    float thSNR;
} cfar2d_filter_cfg_t;

#ifdef __cplusplus
extern "C" {
#endif

cfar2d_result_t *cfar2d_result_alloc(size_t n);

cfar2d_result_t *cfar2d_result_calloc_from_block(void *block, size_t blockSize, size_t n);

void cfar2d_result_free(cfar2d_result_t *result);

int radar_cfar2d_goca(cfar2d_result_t *res, const matrix2d_int32_t *magSepc2D, cfar2d_cfg_t *cfg);
int radar_cfar2d_goca_debug(matrix2d_int32_t *noise, const matrix2d_int32_t *magSepc2D, cfar2d_cfg_t *cfg);

int radar_cfar_result_filtering(cfar2d_result_t *res, cfar2d_filter_cfg_t *cfg);

int cfar2d_result_reset(cfar2d_result_t *cfar);

#ifdef __cplusplus
}
#endif

#endif /* _RADAR_CFAR_H_ */

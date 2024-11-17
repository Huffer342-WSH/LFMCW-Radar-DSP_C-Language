
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "radar_math_types.h"
/**
 * @brief 量测值，包含距离，速度，方位。CFAR检测结果作为补充
 *
 */
typedef struct {
    int32_t distance; // 径向距离(mm)
    int32_t velocity; // 径向速度(mm/s)
    int16_t azimuth;  // 方位角 (Q2.13)
    int32_t amp;
    int32_t snr;
} measurement_t;

/**
 * @brief 量测值，包含距离，速度，方位。CFAR检测结果作为补充
 *
 */
typedef struct {
    measurement_t *data;
    size_t num;
    size_t capacity;
} measurements_t;


typedef struct {
    struct meas_node {
        measurements_t *data;
        struct meas_node *next;
    } head; // 头节点不保存数据，头结点的下一个节点开始正式保存数据
    size_t capacity;
} measurements_list_t;

measurements_t *radar_measurement_alloc(size_t capacity);
void radar_measurements_free(measurements_t *m);

measurements_list_t *radar_measurements_list_alloc(size_t capacity);
void radar_measurements_list_push(measurements_list_t *m, measurements_t *frame);
size_t radar_measurements_list_len(measurements_list_t *m);
size_t radar_measurements_list_get_meas_num(measurements_list_t *m);
void radar_measurements_list_pop(measurements_list_t *m);
void radar_measurements_list_free(measurements_list_t *m);
int radar_measurements_list_copyout(measurements_t *dest, measurements_list_t *m);


#ifdef __cplusplus
}
#endif

/**
 * @file radar_measurement.h
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 量测值结构体的定义
 * @version 0.1
 * @date 2024-11-20
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once


#include "radar_math.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief 单个量测值
 */
typedef struct {
    int32_t distance; ///< 径向距离(mm)
    int32_t velocity; ///< 径向速度(mm/s)
    int16_t azimuth;  ///< 方位角 (Q2.13)
    int32_t amp;      ///< 幅度
    int32_t snr;      ///< 信噪比 (Q23.8)
} measurement_t;


/**
 * @brief 量测值列表，保存多个量测值
 */
typedef struct {
    measurement_t *data; ///< 量测值数组
    size_t num;          ///< 数组中当前的量测值个数
    size_t capacity;     ///< 数组的容量
} measurements_t;


/**
 * @brief 量测值链表，用于保存多帧量测值
 *
 */
typedef struct {
    struct meas_node {
        measurements_t *data;   ///< 量测值数组
        struct meas_node *next; ///< 指向下一个节点
    } head;                     ///< 头节点，本身不保存数据
    size_t capacity;            ///< 链表最大容量
} measurements_list_t;


measurements_t *radar_measurements_alloc(size_t capacity);
measurements_list_t *radar_measurements_list_alloc(size_t capacity);

void radar_measurements_list_free(measurements_list_t *m);
void radar_measurements_free(measurements_t *m);


void radar_measurements_list_push(measurements_list_t *m, measurements_t *frame);
void radar_measurements_list_pop(measurements_list_t *m);
int radar_measurements_list_copyout(measurements_t *dest, measurements_list_t *m);

size_t radar_measurements_list_len(measurements_list_t *m);
size_t radar_measurements_list_get_meas_num(measurements_list_t *m);

int32_t radar_measure_distance(measurement_t *ma, measurement_t *mb, int32_t wr, int32_t wv);

#ifdef __cplusplus
}
#endif

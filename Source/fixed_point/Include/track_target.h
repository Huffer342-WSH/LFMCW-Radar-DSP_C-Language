#pragma once

#include "radar_math.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct target {
    size_t ndim_state;     /// < 维数 N
    uint32_t timestamp_ms; /// < 时间戳 单位毫秒
    double *state_vector;  /// 状态向量 Nx1
    double *covar;         /// 误差协方差矩阵 NxN
} gaussian_state_t;


typedef struct {
    int32_t score;
    uint32_t unassociated_time;
} life_cycle_t;


typedef struct {
    uint32_t uuid;
    gaussian_state_t *state;
    life_cycle_t *life_cycle;
} tracked_target_t;


typedef struct tracked_targets_node {
    tracked_target_t *data;
    struct tracked_targets_node *next;
} tracked_targets_node_t;


typedef struct {
    size_t num;
    tracked_targets_node_t head;
} tracked_targets_list_t;


tracked_targets_list_t *tracked_targets_list_new();

void tracked_targets_node_free(tracked_targets_node_t *node);


void tracked_targets_list_emplace_front(tracked_targets_list_t *list, size_t ndim_state, double *z, uint32_t timestamp_ms);
void tracked_targets_list_remove(tracked_targets_list_t *list, tracked_target_t *target);
void tracked_targets_list_clear(tracked_targets_list_t *list);


#ifdef __cplusplus
}
#endif

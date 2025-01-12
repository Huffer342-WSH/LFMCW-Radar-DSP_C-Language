#pragma once

#include "radar_math.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 被跟踪目标链表
 *
 */
typedef struct {
} tracked_targets_list_t;


/**
 * @brief 被跟踪目标
 *
 */
typedef struct {
} tracked_target_t;


tracked_targets_list_t *tracked_targets_list_new();
void tracked_targets_list_delete(tracked_targets_list_t *list);

tracked_target_t *tracked_targets_list_first(tracked_targets_list_t *list);
tracked_target_t *tracked_targets_list_next(tracked_targets_list_t *list, tracked_target_t *i);

int tracked_target_get_uuid(tracked_target_t *target, uint32_t *uuid);
int tracked_target_get_state_vector(tracked_target_t *target, rd_float_t *state_vector);


/**
 * @brief 遍历链表
 *
 */
#define FOR_EACH_TARGET(pTarget, pTargets) \
    for (tracked_target_t *pTarget = tracked_targets_list_first(pTargets); pTarget != NULL; pTarget = tracked_targets_list_next(pTargets, pTarget))


#ifdef __cplusplus
}
#endif

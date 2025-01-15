#pragma once

#include "radar_math.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 被跟踪目标链表
 *
 * @note 该结构体本身没有意义，在C和C++之间传递使使用的是该结构体的指针
 *
 */
typedef struct {
    char _;
} tracked_targets_list_t;


/**
 * @brief tracked_targets_list_t链表的节点
 *
 * @note 该结构体对应迭代器TrackedTargets::iterator
 *          结构体的数据从迭代器复制而来，即两者在内存中是一致的
 *          因此其大小和对齐方式都要符合list的迭代器的要求
 *
 */
typedef struct {
    uintptr_t ptr;
} tracked_targets_list_node_t;


tracked_targets_list_t *tracked_targets_list_new();
void tracked_targets_list_delete(tracked_targets_list_t *list);

tracked_targets_list_node_t tracked_targets_list_first(tracked_targets_list_t *list);
tracked_targets_list_node_t tracked_targets_list_next(tracked_targets_list_t *list, tracked_targets_list_node_t node);


int tracked_target_get_uuid(tracked_targets_list_node_t node, uint32_t *uuid);
int tracked_target_get_state_vector(tracked_targets_list_node_t node, rd_float_t *state_vector);


/**
 * @brief 遍历链表
 *
 */
#define FOR_EACH_TARGET(pTarget, pTargets) \
    for (tracked_targets_list_node_t pTarget = tracked_targets_list_first(pTargets); pTarget.ptr != 0; pTarget = tracked_targets_list_next(pTargets, pTarget))


#ifdef __cplusplus
}
#endif

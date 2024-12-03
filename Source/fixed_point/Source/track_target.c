/**
 * @file track_target.c
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 保存被跟踪目标学习的结构体以及列表的基本操作
 * @version 0.1
 * @date 2024-12-02
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "track_target.h"
#include <stdlib.h>

gaussian_state_t *gaussian_state_alloc(size_t ndim_state)
{
    gaussian_state_t *state = (gaussian_state_t *)malloc(sizeof(gaussian_state_t));
    state->ndim_state = ndim_state;
    state->state_vector = (double *)malloc(ndim_state * sizeof(double));
    state->covar = (double *)malloc(ndim_state * ndim_state * sizeof(double));
    return state;
}


tracked_target_t *tracked_target_alloc(size_t ndim_state, double *z, uint32_t timestamp_ms)
{
    tracked_target_t *target = (tracked_target_t *)malloc(sizeof(tracked_target_t));
    target->state = gaussian_state_alloc(ndim_state);
    target->life_cycle = (life_cycle_t *)malloc(sizeof(life_cycle_t));


    target->state->timestamp_ms = timestamp_ms;

    // 初始化状态向量
    double phi = z[0], r = z[1], v = z[2];
    target->state->state_vector[0] = r * cos(phi);
    target->state->state_vector[1] = v * cos(phi);
    target->state->state_vector[2] = r * sin(phi);
    target->state->state_vector[3] = v * sin(phi);

    // 初始化误差协方差矩阵
    memset(target->state->covar, 0, ndim_state * ndim_state * sizeof(double));
    target->state->covar[0] = 0.5;
    target->state->covar[5] = 0.1;
    target->state->covar[10] = 0.5;
    target->state->covar[15] = 0.1;

    return target;
}

void tracked_target_free(tracked_target_t *target)
{
    free(target);
    return;
}


void tracked_targets_node_free(tracked_targets_node_t *node)
{
    tracked_target_free(node->data);
    free(node);
    return;
}

tracked_targets_list_t *tracked_targets_list_new()
{
    tracked_targets_list_t *list = (tracked_targets_list_t *)malloc(sizeof(tracked_targets_list_t));
    list->num = 0;
    list->head.data = NULL;
    list->head.next = NULL;
    return list;
}


/**
 * @brief 添加一个目标到列表的开头
 *
 * @param list            列表
 * @param ndim_state      目标状态向量维数
 * @param z               测量值
 * @param timestamp_ms    时间戳
 */
void tracked_targets_list_emplace_front(tracked_targets_list_t *list, size_t ndim_state, double *z, uint32_t timestamp_ms)
{
    tracked_targets_node_t *node = (tracked_targets_node_t *)malloc(sizeof(tracked_targets_node_t));
    node->data = tracked_target_alloc(ndim_state, z, timestamp_ms);
    node->next = list->head.next;
    list->head.next = node;
    list->num++;
    return;
}


/**
 * @brief 从目标列表中删除一个目标
 *
 * @param list      列表
 * @param target    需要删除的目标的地址
 */
void tracked_targets_list_remove(tracked_targets_list_t *list, tracked_target_t *target)
{
    tracked_targets_node_t *prev = &list->head;
    tracked_targets_node_t *cur = prev->next;

    while (cur) {
        if (cur->data == target) { // 地址一样
            tracked_targets_node_t *next = cur->next;
            tracked_targets_node_free(cur); // 释放节点
            prev->next = next;
            list->num--;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    return;
}


void tracked_targets_list_clear(tracked_targets_list_t *list)
{
    tracked_targets_node_t *node = list->head.next;
    while (node != NULL) {
        tracked_targets_node_t *tmp = node;
        node = node->next;
        tracked_target_free(tmp->data);
        free(tmp);
    }
    free(list);
    return;
}

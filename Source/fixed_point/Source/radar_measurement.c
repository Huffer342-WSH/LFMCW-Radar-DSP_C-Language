/**
 * @file radar_measurement.c
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 量测值的分配和释放, 以及量测值之间距离的计算
 * @version 0.1
 * @date 2024-11-20
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "radar_measurement.h"

#include "radar_assert.h"
#include "radar_error.h"

#include <stdlib.h>
#include <string.h>

/**
 * @brief 分配一个量测值列表，可容纳capacity个量测值
 *
 * @param[in]  capacity  量测值列表的容量
 * @return               量测值指针
 *                         - NULL  分配失败
 */
measurements_t *radar_measurements_alloc(size_t capacity)
{
    measurements_t *meas = (measurements_t *)malloc(sizeof(measurements_t));
    if (meas == NULL) {
        RADAR_ERROR("failed to allocate space for measurements_t", RADAR_ENOMEM);
        return NULL;
    }

    meas->data = (measurement_t *)malloc(sizeof(measurement_t) * capacity);
    if (meas->data == NULL) {
        free(meas);
        RADAR_ERROR("failed to allocate space for measurements_t", RADAR_ENOMEM);
        return NULL;
    }

    meas->num = 0;
    meas->capacity = capacity;
    return meas;
}


void radar_measurements_free(measurements_t *m)
{
    free(m->data);
    free(m);
}


measurements_list_t *radar_measurements_list_alloc(size_t capacity)
{
    measurements_list_t *m = (measurements_list_t *)malloc(sizeof(measurements_list_t));
    if (m == NULL) {
        RADAR_ERROR("failed to allocate space for measurements_list_t", RADAR_ENOMEM);
        return NULL;
    }
    m->head.data = NULL;
    m->head.next = NULL;
    m->capacity = capacity;

    return m;
}

static void meas_node_free(struct meas_node *node)
{
    radar_measurements_free(node->data);
    free(node);
}

static void delete_all_meas_node(struct meas_node *node)
{
    if (node == NULL) {
        return;
    }
    struct meas_node *next;
    do {
        next = node->next;
        meas_node_free(node);
        node = next;
    } while (next != NULL);
}

void measurements_list_delete_tail(measurements_list_t *m, size_t keepsize)
{
    struct meas_node *tail = &m->head;
    size_t cnt = 0;
    while (tail->next != NULL && cnt < keepsize) {
        cnt++;
        tail = tail->next;
    }

    if (cnt >= keepsize) {
        delete_all_meas_node(tail->next);
        tail->next = NULL;
    }
}

void radar_measurements_list_push(measurements_list_t *m, measurements_t *frame)
{
    /* 已满自动删除尾部 */
    measurements_list_delete_tail(m, m->capacity - 1);

    /* 头部插入新节点 */
    struct meas_node *node = (struct meas_node *)malloc(sizeof(struct meas_node));
    if (node == NULL) {
        RADAR_ERROR("failed to allocate space for meas_node", RADAR_ENOMEM);
        return;
    }
    node->data = radar_measurements_alloc(frame->num);
    memcpy(node->data->data, frame->data, sizeof(measurement_t) * frame->num);
    node->data->num = frame->num;
    node->next = m->head.next;

    m->head.next = node;
}

size_t radar_measurements_list_len(measurements_list_t *m)
{
    struct meas_node *node = m->head.next;
    size_t cnt = 0;
    while (node != NULL) {
        cnt++;
        node = node->next;
    }
    return cnt;
}

size_t radar_measurements_list_get_meas_num(measurements_list_t *m)
{
    struct meas_node *node = m->head.next;
    size_t cnt = 0;
    while (node != NULL) {
        cnt += node->data->num;
        node = node->next;
    }
    return cnt;
}


void radar_measurements_list_pop(measurements_list_t *m)
{
    struct meas_node *new_tail = &m->head;
    struct meas_node *tail = &m->head;


    /*  寻找尾节点 */
    while (tail->next != NULL) {
        new_tail = tail;
        tail = new_tail->next;
    }
    /* 释放尾节点 */
    if (tail != new_tail) {
        meas_node_free(tail);
        new_tail->next = NULL;
    }
    return;
}


void radar_measurements_list_free(measurements_list_t *m)
{
    struct meas_node *node = m->head.next;
    while (node != NULL) {
        struct meas_node *tmp = node;
        node = node->next;
        radar_measurements_free(tmp->data);
        free(tmp);
    }
    free(m);
}


int radar_measurements_list_copyout(measurements_t *dest, measurements_list_t *m)
{
    struct meas_node *node;
    node = m->head.next;
    dest->num = 0;
    while (node != NULL) {
        size_t free_size = dest->capacity - dest->num;
        size_t copy_size = node->data->num;
        if (free_size < copy_size) {
            return -1;
        }
        memcpy(dest->data + dest->num, node->data->data, sizeof(measurement_t) * node->data->num);
        dest->num += node->data->num;
        node = node->next;
    }
    return 0;
}


/**
 * @brief 计算两个两侧值之间的距离（包含速度，视为三维空间）
 *
 * @param ma 量侧值a
 * @param mb 量测值b
 * @param wr 二位平面距离权重
 * @param wv 速度权重
 * @return int32_t 距离
 *
 * @note 量测值的数值太大会导致溢出，实际应用中不会有这么大的数据
 */
int32_t radar_measure_distance(measurement_t *ma, measurement_t *mb, int32_t wr, int32_t wv)
{

    int32_t v = abs_diff(ma->velocity, mb->velocity);

    // c^ = a^2 + b^2 - 2 * a * b * cos(theta)
    int32_t a = ma->distance;
    int32_t b = mb->distance;
    int32_t theta_q13 = (int32_t)ma->azimuth - mb->azimuth;
    if (theta_q13 < -PI_Q13) {
        theta_q13 += 2 * PI_Q13;
    } else if (theta_q13 > PI_Q13) {
        theta_q13 = 2 * PI_Q13 - theta_q13;
    }
    int32_t norm_theta_q31 = (int32_t)((int64_t)theta_q13 * ((int64_t)1 << 30) / (PI_Q13));
    int64_t c2;
    c2 = (int64_t)a * radar_cos_q31(norm_theta_q31) >> 31;
    c2 *= (int64_t)b * -2;
    c2 += (int64_t)a * a + b * b;

    int64_t sum;

    int64_t wr2_q31 = ((int64_t)wr * wr) >> 1;
    int64_t wv2_q31 = ((int64_t)wv * wv) >> 1;
    sum = (wr2_q31 * c2 + wv2_q31 * v * v) >> 30; // 少右移一位，即乘以2，配合radar_sqrt_q31后面的右移16位抵消radar_sqrt_q31()多乘的sqrt(2^31)
    if (sum < 0 || sum > INT32_MAX) {
        RADAR_ERROR("radar_cluster_calc_distance overflow", RADAR_EOVRFLW);
    }
    return radar_sqrt_q31((int32_t)sum) >> 16;
}

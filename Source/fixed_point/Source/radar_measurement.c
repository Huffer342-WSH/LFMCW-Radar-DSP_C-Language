#include "radar_measurement.h"
#include "radar_assert.h"
#include "radar_error.h"

#include <stdlib.h>
#include <string.h>

measurements_t *radar_measurements_alloc(size_t capacity)
{
    measurements_t *meas = (measurements_t *)malloc(sizeof(measurements_t));
    meas->data = (measurement_t *)malloc(sizeof(measurement_t) * capacity);
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

void delete_all_meas_node(struct meas_node *node)
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

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
#include <radar/sp/fixed_point/radar_measurement.h>

#include <radar/common/radar_log.h>
#include <radar/lib/bitset.h>

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
    measurements_t *meas = (measurements_t *)rd_malloc(sizeof(measurements_t));
    if (meas == NULL) {
        RADAR_ERROR("failed to allocate space for measurements_t", RADAR_ENOMEM);
        return NULL;
    }

    meas->data = (measurement_t *)rd_malloc(sizeof(measurement_t) * capacity);
    if (meas->data == NULL) {
        rd_free(meas);
        RADAR_ERROR("failed to allocate space for measurements_t", RADAR_ENOMEM);
        return NULL;
    }

    meas->num = 0;
    meas->capacity = capacity;
    return meas;
}


void radar_measurements_free(measurements_t *m)
{
    rd_free(m->data);
    rd_free(m);
}


measurements_list_t *radar_measurements_list_alloc(size_t capacity)
{
    measurements_list_t *m = (measurements_list_t *)rd_malloc(sizeof(measurements_list_t));
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
    rd_free(node);
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
    struct meas_node *node = (struct meas_node *)rd_malloc(sizeof(struct meas_node));
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
        rd_free(tmp);
    }
    rd_free(m);
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
    sum = (wr2_q31 * c2 + wv2_q31 * v * v) >>
        30; // 少右移一位，即乘以2，配合radar_sqrt_q31后面的右移16位抵消radar_sqrt_q31()多乘的sqrt(2^31)
    if (sum < 0 || sum > INT32_MAX) {
        RADAR_ERROR("radar_cluster_calc_distance overflow", RADAR_EOVRFLW);
    }
    return radar_sqrt_q31((int32_t)sum) >> 16;
}


/**
 * @brief  删除被遮挡的量测值
 *
 * @param[out]  meas  量测值列表
 * @param[in]   r     量测值半径
 * @return int
 *
 * @details  被遮挡的量测值将被删除，水平安装雷达时可以使用
 *
 */
int radar_measure_delete_obscured(measurements_t *meas, int32_t r)
{
    if (r == 0) {
        return 0;
    }
    r = abs(r);

    const size_t n = meas->num;
    bitset_t *mask = bitset_new(n, true);

    for (size_t i = 0; i < n; i++) {
        int32_t occluded_angle_range = INT32_MAX;
        measurement_t *a = &meas->data[i];
        for (size_t j = 0; j < n; j++) {
            if (j == i)
                continue;
            measurement_t *b = &meas->data[j];
            if (a->distance < b->distance && b->velocity == 0) {
                if (occluded_angle_range == INT32_MAX) {
                    occluded_angle_range = radar_atan2_q31(r, a->distance) >> 16;
                }
                int32_t angle_diff = (int32_t)a->azimuth - (int32_t)b->azimuth;
                angle_diff = abs(angle_diff);
                if (occluded_angle_range > angle_diff) {
                    bitset_reset(mask, j);
                }
            }
        }
    }
    size_t numPoint = 0;
    for (size_t i = 0; i < n; i++) {
        if (bitset_test(mask, i)) {
            if (numPoint != i)
                meas->data[numPoint] = meas->data[i];
            numPoint++;
        }
    }
    meas->num = numPoint;
    bitset_delete(mask);
    return 0;
}


/**
 * @brief 分配一个量测值缓冲区
 *
 * @param gp_size 最多可以存储多少帧量测值
 * @param m_size  最多可以存储多少个量测值
 * @return measurements_buffer_t*
 */
measurements_buffer_t *radar_measurements_buffer_alloc(size_t gp_size, size_t m_size)
{
    measurements_buffer_t *buf;

    gp_size++;
    m_size++;

    buf = (measurements_buffer_t *)malloc(sizeof(measurements_buffer_t));
    if (!buf)
        return NULL;

    buf->gp_fifo = (size_t *)malloc(sizeof(size_t) * gp_size);
    if (!buf->gp_fifo) {
        free(buf);
        return NULL;
    }

    buf->meas_fifo = (measurement_t *)malloc(sizeof(measurement_t) * m_size);
    if (!buf->meas_fifo) {
        free(buf->gp_fifo);
        free(buf);
        return NULL;
    }

    buf->gp_size = gp_size;
    buf->m_size = m_size;
    buf->gp_in = buf->gp_out = 0;
    buf->m_in = buf->m_out = 0;

    return buf;
}

/**
 * @brief 释放量测值缓冲区
 *
 * @param buf 量测值缓冲区
 */
void radar_measurements_buffer_free(measurements_buffer_t *buf)
{
    RADAR_ASSERT(buf != NULL && buf->gp_fifo != NULL && buf->meas_fifo != NULL);
    free(buf->gp_fifo);
    free(buf->meas_fifo);
    free(buf);
}

/**
 * @brief 计算 meas 队列可用空间
 */
static size_t meas_available_space(measurements_buffer_t *buf)
{
    if (buf->m_in >= buf->m_out)
        return buf->m_size - (buf->m_in - buf->m_out);
    else
        return buf->m_out - buf->m_in;
}

/**
 * @brief 计算 gp_fifo 队列可用空间
 */
static size_t gp_available_space(measurements_buffer_t *buf)
{
    if (buf->gp_in >= buf->gp_out)
        return buf->gp_size - (buf->gp_in - buf->gp_out);
    else
        return buf->gp_out - buf->gp_in;
}

/**
 * @brief 入队一帧 measurement
 *
 * @param buf   量测值缓冲区
 * @param frame 量测值帧
 * @return int
 */
int radar_measurements_buffer_push(measurements_buffer_t *buf, measurements_t *frame)
{
    size_t n;
    n = frame->num;

    RADAR_ASSERT(buf && frame);

    if (gp_available_space(buf) == 0 || meas_available_space(buf) < n) {
        return -1; // 空间不足
    }

    size_t first_part = buf->m_size - buf->m_in;
    if (first_part >= n) {
        memcpy(&buf->meas_fifo[buf->m_in], frame->data, n * sizeof(measurement_t));
        buf->m_in = (buf->m_in + n) % buf->m_size;
    } else {
        // 分两次拷贝以处理循环队列 wrap-around
        memcpy(&buf->meas_fifo[buf->m_in], frame->data, first_part * sizeof(measurement_t));
        memcpy(&buf->meas_fifo[0], frame->data + first_part,
               (n - first_part) * sizeof(measurement_t));
        buf->m_in = n - first_part;
    }

    buf->gp_fifo[buf->gp_in] = n;
    buf->gp_in = (buf->gp_in + 1) % buf->gp_size;

    return 0;
}

/**
 * @brief 出队一帧 measurement
 *
 * @param buf
 * @return int
 */
int radar_measurements_buffer_pop(measurements_buffer_t *buf)
{
    RADAR_ASSERT(buf);

    if (buf->gp_out == buf->gp_in)
        return -1; // 队列空

    size_t frame_num = buf->gp_fifo[buf->gp_out];
    buf->gp_out = (buf->gp_out + 1) % buf->gp_size;
    buf->m_out = (buf->m_out + frame_num) % buf->m_size;

    return 0;
}

/**
 * @brief 从量测值缓冲区中拷贝出量测值到目标量测值数组
 *
 * @param dest 目标量测值数组
 * @param m    量测值缓冲区
 * @return int
 */
int radar_measurements_buffer_copyout(measurements_t *dest, measurements_buffer_t *buf)
{
    if (!dest || !buf)
        return -1;

    size_t total = 0;        /* 总共要拷贝的 measurement 数量 */
    size_t idx = buf->gp_in; /* 临时索引，用于从 gp_in 往 gp_out 回溯 */

    /* 先计算可以完整拷贝的帧数，总量不超过 dest->capacity */
    while (idx != buf->gp_out) {
        /* 前向索引，靠近 m_in */
        idx = (idx == 0) ? buf->gp_size - 1 : idx - 1;
        size_t frame_size = buf->gp_fifo[idx];
        if (total + frame_size > dest->capacity)
            break; /* 超过 dest 容量则停止 */
        total += frame_size;
        if (idx == buf->gp_out)
            break;
    }

    if (total == 0) {
        dest->num = 0; /* 没有可拷贝的内容 */
        return 0;
    }

    /* 计算 meas 中 measurement 数据的起始和结束索引 */
    size_t meas_end = buf->m_in;
    size_t meas_start = (meas_end >= total) ? meas_end - total : buf->m_size + meas_end - total;

    /* 一次或两次 memcpy 拷贝，处理循环队列 wrap-around */
    if (meas_start + total <= buf->m_size) {
        /* 不 wrap-around，一次 memcpy */
        memcpy(dest->data, &buf->meas_fifo[meas_start], total * sizeof(measurement_t));
    } else {
        /* wrap-around，分两次 memcpy */
        size_t part = buf->m_size - meas_start;
        memcpy(dest->data, &buf->meas_fifo[meas_start], part * sizeof(measurement_t));
        memcpy(dest->data + part, &buf->meas_fifo[0], (total - part) * sizeof(measurement_t));
    }

    dest->num = total; /* 设置实际拷贝数量 */
    return 0;
}

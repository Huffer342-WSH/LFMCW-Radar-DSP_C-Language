/**
 * @file radar_cluster.c
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 聚类，暂时只实现DBSCAN
 *
 * @note DBSCAN需要查询邻居信息，即查询一个点的所有邻居。
 *       这个步骤可以是只计算邻接矩阵D保存任意两点之间的距离，然后每次都从D中遍历得到邻居，这种方法在重复查询时浪费的时间多，但是消耗内存小且固定
 *       也可以是计算任意两点之间的距离时就保存邻居信息，查询的时候直接返回已有的邻居信息即可，但是保存所有邻居信息需要的内存不固定，最大n(n-1)
 *
 *       目前采用的是第二种方法，因为对于室内雷达来说，点云数量很少
 * @version 0.1
 * @date 2024-11-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "radar_cluster.h"

#include "radar_error.h"
#include "radar_assert.h"

#include <stdlib.h>
#include <stdio.h>


/**
 * @brief DBSCAN核心
 *
 * @param[out] labels 标签，-1代表杂点
 * @param n 样本数量
 * @param eps 领域半径，Epsilon
 * @param min_samples 核心的最小样本数
 * @param get_neighbors 回调函数，get_neighbors(&count, i, eps, param) 返回保存邻居的序号的数组，count 为邻居个数
 * @param param 回调函数的额外参数
 */
int dbscan_core(size_t *labels, size_t n, int32_t eps, size_t min_samples, cb_get_neighbors_t get_neighbors, void *param)
{
    size_t *stack;
    size_t stack_top = 0;
    size_t *neighbors = NULL;
    size_t count = 0;
    size_t label_num = 0;
    if (n == 0) {
        return 0;
    }
    if (n == 1) {
        if (min_samples == 0) {
            labels[0] = 0;
            return 1;
        } else {
            labels[0] = SIZE_MAX;
            return 0;
        }
    }
    stack = (size_t *)malloc((n - 1) * sizeof(size_t));
    if (stack == NULL) {
        RADAR_ERROR("dbscan_core malloc stack failed", RADAR_ENOMEM);
        return -1;
    }


    memset(labels, 0xFFFF, n * sizeof(labels[0]));

    for (size_t i = 0; i < n; i++) {
        if (labels[i] != SIZE_MAX) {
            continue;
        }
        neighbors = get_neighbors(&count, i, eps, param);
        if (count < min_samples)
            continue;
        size_t j = i;
        labels[j] = label_num;
        while (1) {
            neighbors = get_neighbors(&count, j, eps, param);
            if (count >= min_samples) {
                for (size_t k = 0; k < count; k++) {
                    size_t v = neighbors[k];
                    if (v >= n) { }
                    if (labels[v] == SIZE_MAX) {
                        stack[stack_top++] = v;
                        labels[v] = label_num;
                    }
                }
            }

            if (stack_top == 0) {
                break;
            }
            j = stack[--stack_top];
        }
        label_num++;
    }
    free(stack);
    return label_num;
}

/**
 * @brief  计算量测值列表任意两点之间的距离
 *
 * @param meas 量侧值列表
 * @param wr Q15.16 平面距离的权重
 * @param wv Q15.16 速度的权重
 * @param D 保存量测点之间距离的数组，长度为N*(N-1)/2，需要手动释放。
 *          排列顺序为
 *          0:(1,0)
 *          1:(2,0) 2:(2,1)
 *          3:(3,0) 4:(3,1) 5:(3,2)
 *          ...
 *          即(i,j)对应序号为 i*(i-1)/2 + j
 */
static void radar_calc_meas_distance(measurements_t *meas, int32_t wr, int32_t wv, int32_t *D)
{
    // d = sqrt( (c*wr)^2 + (v*wv)^2 )/(wr + wv)
    const size_t n = meas->num;
    for (size_t i = 1; i < n; i++) {
        const size_t offset = i * (i - 1) / 2;
        for (size_t j = 0; j < i; j++) {
            D[offset + j] = radar_measure_distance(&meas->data[i], &meas->data[j], wr, wv);
        }
    }
}

/**
 * @brief  计算每个点的邻居数量
 *
 * @param n 量测值的数量
 * @param D 保存量测点之间距离的数组，和radar_calc_meas_distance()函数的D参数相同
 * @param eps DBSCAN的领域大小
 * @param n_neighbors 保存每个点的邻居数量的数组，元素个数为n
 */
static void radar_calc_neighbors_num(size_t n, int32_t *D, int32_t eps, size_t *n_neighbors)
{
    for (size_t i = 1; i < n; i++) {
        for (size_t j = 0; j < i; j++) {
            if (D[i * (i - 1) / 2 + j] <= eps) {
                n_neighbors[i]++;
                n_neighbors[j]++;
            }
        }
    }
}


/**
 * @brief  计算每个点的邻居信息
 *
 * @param n 量测值的数量
 * @param D 保存量测点之间距离的数组，和radar_calc_meas_distance()函数的D参数相同
 * @param eps DBSCAN的领域大小
 * @param neighborhoods 保存每个点的邻居信息的数组，元素个数为n * n
 *
 * @note  neighborhoods[i]表示i号点的邻居信息
 */
static void radar_calc_neighbors_info(size_t n, int32_t *D, int32_t eps, size_t **neighborhoods)
{
    for (size_t i = 0; i < n; i++) {
        size_t cnt = 0;
        for (size_t j = 0; j < i; j++) {
            if (D[i * (i - 1) / 2 + j] <= eps) {
                neighborhoods[i][cnt++] = j;
            }
        }
        for (size_t j = i + 1; j < n; j++) {
            if (D[j * (j - 1) / 2 + i] <= eps) {
                neighborhoods[i][cnt++] = j;
            }
        }
    }
}


/**
 * @brief  创建DBSCAN聚类的邻居信息
 *
 * @param[in] meas 量测值列表
 * @param[in] wr  DBSCAN的领域半径
 * @param[in] wv  DBSCAN的领域半径
 * @param[in] eps DBSCAN的领域大小
 * @return dbscan_neighbors_t*  Created dbscan_neighbors_t
 *
 * @note  该函数会分配内存，需要调用radar_cluster_dbscan_neighbors_free()来释放
 */
dbscan_neighbors_t *radar_cluster_dbscan_neighbors_create(measurements_t *meas, int32_t wr, int32_t wv, int32_t eps)
{
    dbscan_neighbors_t *nb = (dbscan_neighbors_t *)malloc(sizeof(dbscan_neighbors_t));
    size_t i;
    int failed_number = 0;
    if (nb == NULL) {
        RADAR_ERROR("radar_cluster_dbscan_neighbors_create malloc dbscan_neighbors_t failed", RADAR_ENOMEM);
        return NULL;
    }
    size_t capacity = meas->num * (meas->num - 1) / 2;
    nb->n = meas->num;

    /* 距离矩阵 */
    nb->D = (int32_t *)malloc(capacity * sizeof(nb->D[0]));
    if (nb->D == NULL) {
        RADAR_ERROR("radar_cluster_dbscan_neighbors_create malloc D failed", RADAR_ENOMEM);
        failed_number = 1;
    }
    radar_calc_meas_distance(meas, wr, wv, nb->D);


    /* 邻居数量 */
    nb->n_neighbors = (size_t *)calloc(nb->n, sizeof(nb->n_neighbors[0]));
    if (nb->n_neighbors == NULL) {
        RADAR_ERROR("radar_cluster_dbscan_neighbors_create malloc n_neighbors failed", RADAR_ENOMEM);
        failed_number = 2;
    }
    radar_calc_neighbors_num(nb->n, nb->D, eps, nb->n_neighbors);

    /* 邻居信息 */
    nb->neighborhoods = (size_t **)malloc(nb->n * sizeof(nb->neighborhoods[0]));
    if (nb->neighborhoods == NULL) {
        RADAR_ERROR("radar_cluster_dbscan_neighbors_create malloc neighborhoods failed", RADAR_ENOMEM);
        failed_number = 3;
    }
    for (i = 0; i < nb->n; i++) {
        nb->neighborhoods[i] = (size_t *)malloc(nb->n_neighbors[i] * sizeof(nb->neighborhoods[0][0]));
        if (nb->neighborhoods[i] == NULL) {
            RADAR_ERROR("radar_cluster_dbscan_neighbors_create malloc failed", RADAR_ENOMEM);
            failed_number = 4;
            break;
        }
    }
    radar_calc_neighbors_info(nb->n, nb->D, eps, nb->neighborhoods);

    /* 发生分配失败时，释放已经成功分配的内存 */
    switch (failed_number) {
    case 4:
        for (size_t j = 0; j < i; j++) free(nb->neighborhoods[j]);
    case 3:
        free(nb->neighborhoods);
    case 2:
        free(nb->n_neighbors);
    case 1:
        free(nb->D);
        break;
    default:
        break;
    }

    return nb;
}

/**
 * @brief 释放 radar_cluster_dbscan_neighbors_create 生成的对象
 *
 * @param nb radar_cluster_dbscan_neighbors_create 生成的对象
 */
void radar_cluster_dbscan_neighbors_free(dbscan_neighbors_t *nb)
{
    for (size_t i = 0; i < nb->n; i++) {
        free(nb->neighborhoods[i]);
    }
    free(nb->neighborhoods);
    free(nb->n_neighbors);
    free(nb->D);
    free(nb);
}


/**
 * @brief 获取一个点的邻居，系统默认回调函数
 *
 * @param count 邻居的数量
 * @param idx 待查询的点
 * @param eps 领域大小
 * @param param 额外参数
 * @return size_t*
 */
size_t *radar_get_neighbors(size_t *count, size_t idx, int32_t eps, void *param)
{
    dbscan_neighbors_t *nb = (dbscan_neighbors_t *)param;
    *count = nb->n_neighbors[idx];
    return nb->neighborhoods[idx];
}

/**
 * @brief 对量测值进行DBSCAN聚类
 *
 * @param[out] labels 每个量测值的聚类标签
 * @param meas 量测值列表
 * @param wr 平面距离的权重
 * @param wv 速度的权重
 * @param eps DBSCAN的领域大小
 * @param min_samples DBSCAN的核心点需要的最小邻居数
 */
int radar_cluster_dbscan(size_t *labels, measurements_t *meas, int32_t wr, int32_t wv, int32_t eps, size_t min_samples)
{
    int num_cluster;
    dbscan_neighbors_t *nb = radar_cluster_dbscan_neighbors_create(meas, wr, wv, eps);
    if (nb == NULL) {
        RADAR_ERROR("radar_cluster_dbscan malloc dbscan_neighbors_t failed", RADAR_ENOMEM);
        return -1;
    }
    num_cluster = dbscan_core(labels, meas->num, eps, min_samples, radar_get_neighbors, nb);
    radar_cluster_dbscan_neighbors_free(nb);
    return num_cluster;
}


/**
 * @brief 对DBSCAN聚类结果进行融合，计算每个聚类的均值
 *
 * @param[out] clusters 保存聚类的均值结果
 * @param num_cluster 聚类的数量
 * @param labels 量测值的聚类标签
 * @param meas 量测值列表
 */
int radar_cluster_fusion(measurements_t *clusters, size_t num_cluster, size_t *labels, measurements_t *meas)
{
    RADAR_ASSERT(clusters != NULL && clusters->capacity >= num_cluster);
    for (size_t i = 0; i < num_cluster; i++) {
        int64_t distance, velocity, azimuth, amp, snr;
        int64_t cnt = 0;
        amp = distance = velocity = azimuth = snr = 0;
        for (size_t j = 0; j < meas->num; j++) {
            if (labels[j] != i)
                continue;
            measurement_t *m = &meas->data[j];
            amp += (int64_t)m->amp;
            distance += (int64_t)m->distance;
            velocity += (int64_t)m->velocity;
            azimuth += (int64_t)m->azimuth;
            snr += (int64_t)m->snr;
            cnt++;
        }
        measurement_t *m = &clusters->data[i];
        m->amp = amp / cnt;
        m->distance = distance / cnt;
        m->velocity = velocity / cnt;
        m->azimuth = azimuth / cnt;
        m->snr = snr / cnt;
    }
    clusters->num = num_cluster;
    return 0;
}

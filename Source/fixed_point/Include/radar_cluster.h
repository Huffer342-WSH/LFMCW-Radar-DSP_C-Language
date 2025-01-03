#pragma once

#include "radar_math.h"
#include "radar_measurement.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 保存DBSCAN邻居信息的结构体
 *
 */
typedef struct {
    size_t n;               ///< 待聚类点数
    int32_t *D;             ///< 保存量测点之间距离的数组
    size_t **neighborhoods; ///< 保存每个点的邻居信息的数组
    size_t *n_neighbors;    ///< 保存每个点的邻居数量的数组
} dbscan_neighbors_t;

/// @brief DBSCAN的邻居获取回调函数
typedef size_t *(*cb_get_neighbors_t)(size_t *count, size_t idx, int32_t eps, void *param);


/**
 * @brief DBSCAN核心
 *
 * @param[out] labels  量测点的聚类标签
 * @param n  量测点的数量
 * @param eps  DBSCAN的领域半径
 * @param min_samples  DBSCAN的核心的最小样本数
 * @param get_neighbors  回调函数，get_neighbors(&count, i, eps, param) 返回保存邻居的序号的数组，count 为邻居个数
 * @param param  回调函数的额外参数
 */
int dbscan_core(size_t *labels, size_t n, int32_t eps, size_t min_samples, cb_get_neighbors_t get_neighbors, void *param);

/**
 * @brief  创建DBSCAN聚类的邻居信息
 *
 * @note  该函数会将速度也当成一个维度计算，通过权重控制速度和距离的影响程度。
 *
 * @param[in] meas  量测值列表
 * @param[in] wr  距离权重
 * @param[in] wv  速度权重
 * @param[in] eps  DBSCAN的领域大小
 * @return dbscan_neighbors_t*  Created dbscan_neighbors_t
 *
 * @note  该函数会分配内存，需要调用radar_cluster_dbscan_neighbors_free()来释放
 */
dbscan_neighbors_t *radar_cluster_dbscan_neighbors_create(measurements_t *meas, int32_t wr, int32_t wv, int32_t eps);

/**
 * @brief  释放DBSCAN聚类的邻居信息
 *
 * @param[in] nb  需要释放的dbscan_neighbors_t
 */
void radar_cluster_dbscan_neighbors_free(dbscan_neighbors_t *nb);


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
int radar_cluster_dbscan(size_t *labels, measurements_t *meas, int32_t wr, int32_t wv, int32_t eps, size_t min_samples);


/**
 * @brief 对DBSCAN聚类结果进行融合，计算每个聚类的均值
 *
 * @param[out] clusters 保存聚类的均值结果
 * @param num_cluster 聚类的数量
 * @param labels 量测值的聚类标签
 * @param meas 量测值列表
 */
int radar_cluster_fusion(measurements_t *clusters, size_t num_cluster, size_t *labels, measurements_t *meas);

#ifdef __cplusplus
}
#endif

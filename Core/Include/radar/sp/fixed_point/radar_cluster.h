#pragma once

#include <radar/sp/fixed_point/radar_math.h>
#include <radar/sp/fixed_point/radar_measurement.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 保存DBSCAN邻居信息的结构体
 *
 */
typedef struct {
    size_t capacity;     ///< 容量
    size_t n;            ///< 待聚类点数
    int32_t *D;          ///< 保存量测点之间距离的数组
    size_t *n_neighbors; ///< 保存每个点的邻居数量的数组
} dbscan_neighbors_t;


/**
 * @brief  回调函数，get_neighbors(&count, i, eps, param) 返回保存邻居的序号的数组，count 为邻居个数
 *
 * @param[out] count  保存邻居个数的指针
 * @param idx  量测点的序号
 * @param eps  DBSCAN的领域半径
 * @param param  回调函数的额外参数
 * @return  0表示成功
 */
typedef int (*dbscan_get_nbcnt_cb)(size_t *count, size_t idx, int32_t eps, void *param);

/**
 * @brief  回调函数，get_neighbors(&count, i, eps, param) 返回保存邻居的序号的数组，count 为邻居个数
 *
 * @param[out] neighbors  保存邻居的序号的数组
 * @param size  保存邻居的序号的数组大小
 * @param idx  量测点的序号
 * @param eps  DBSCAN的领域半径
 * @param param  回调函数的额外参数
 * @return  0表示成功
 */
typedef int (*dbscan_get_nbinfo_cb)(size_t *neighbors, size_t size, size_t idx, int32_t eps,
                                    void *param);

/**
 * @brief  DBSCAN核心
 *
 * @param[out] labels  量测点的聚类标签
 * @param[out] n_labels  聚类的数量
 * @param n  量测点的数量
 * @param eps  DBSCAN的领域半径
 * @param min_samples  DBSCAN的核心的最小样本数
 * @param get_n  回调函数，返回一个点的邻居个数
 * @param get_neighbors  回调函数，返回一个点的所有邻居
 * @param param  回调函数的额外参数
 * @return 0表示成功，-1/-2 表示分配内存失败
 */
int dbscan_core(size_t *labels, size_t *n_labels, size_t n, int32_t eps, size_t min_samples,
                dbscan_get_nbcnt_cb get_n, dbscan_get_nbinfo_cb get_neighbors, void *param);

/**
 * @brief  分配 DBSCAN 邻居信息结构
 *
 * @param[in] n 量测点数量
 * @return dbscan_neighbors_t*  Allocated dbscan_neighbors_t (未初始化数据)
 *
 * @note  仅负责内存分配和基础初始化，
 *        使用完成后需调用 radar_cluster_dbscan_neighbors_free() 释放。
 */
dbscan_neighbors_t *radar_cluster_dbscan_neighbors_alloc(size_t n);

/**
 * @brief  初始化 DBSCAN 邻居信息
 *
 * @param[in,out] nb   已分配的邻居结构（容量固定）
 * @param[in]     meas 量测值列表
 * @param[in]     wr   DBSCAN的领域半径
 * @param[in]     wv   DBSCAN的速度半径
 * @param[in]     eps  DBSCAN的邻域阈值
 * @return 0 成功, -1 容量不足（仅部分初始化）, -2 参数无效
 *
 * @note  如果 meas->num > nb->n，则只处理前 nb->n 个量测点，
 *        并返回 -1。上层应视情况重新分配更大的 nb。
 */
int radar_cluster_dbscan_neighbors_init(dbscan_neighbors_t *nb, measurements_t *meas, int32_t wr,
                                        int32_t wv, int32_t eps);

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
dbscan_neighbors_t *radar_cluster_dbscan_neighbors_create(measurements_t *meas, int32_t wr,
                                                          int32_t wv, int32_t eps);

/**
 * @brief  释放DBSCAN聚类的邻居信息
 *
 * @param[in] nb  需要释放的dbscan_neighbors_t
 */
void radar_cluster_dbscan_neighbors_free(dbscan_neighbors_t *nb);


/**
 * @brief  DBSCAN聚类
 *
 * @param[in,out] nb  dbscan_neighbors_t
 * @param[out] labels  量测点的聚类标签
 * @param[out] n_labels  聚类的数量
 * @param[in] meas  量测值列表
 * @param[in] wr  DBSCAN的领域半径
 * @param[in] wv  DBSCAN的速度半径
 * @param[in] eps  DBSCAN的邻域阈值
 * @param[in] min_samples  DBSCAN的核心的最小样本数
 * @return  OK表示成功
 *          -RADAR_ENOMEM 内存分配失败
 *          -RADAR_ECAPACITY 容量不足（仅部分初始化);
 *          -RADAR_EINVAL 参数无效;
 *
 * @note  dbscan_neighbors_t *nb需要通过 radar_cluster_dbscan_neighbors_alloc() 预分配内存
 */
int radar_cluster_dbscan(dbscan_neighbors_t *nb, size_t *labels, size_t *n_labels,
                         measurements_t *meas, int32_t wr, int32_t wv, int32_t eps,
                         size_t min_samples);

/**
 * @brief 对DBSCAN聚类结果进行融合，计算每个聚类的均值
 *
 * @param[out] clusters 保存聚类的均值结果
 * @param num_cluster 聚类的数量
 * @param labels 量测值的聚类标签
 * @param meas 量测值列表
 */
int radar_cluster_fusion(measurements_t *clusters, size_t num_cluster, size_t *labels,
                         measurements_t *meas);

#ifdef __cplusplus
}
#endif

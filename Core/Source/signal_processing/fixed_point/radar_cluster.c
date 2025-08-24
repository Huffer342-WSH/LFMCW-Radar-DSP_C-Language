/**
 * @file radar_cluster.c
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 聚类，暂时只实现DBSCAN
 *
 * @note DBSCAN是一个深度搜索，继续遍历的条件是邻居数量大于等于min_samples
 *       算法流程简述：
 *       1. 初始化所有点的标签为未访问。
 *       2. 遍历每个点：
 *          - 若该点未被访问，则查找其邻居数量。
 *          - 若邻居数小于 min_samples，则标记为噪声。
 *          - 否则创建一个新簇，将该点及其邻居加入簇中。
 *       3. 通过邻居扩展不断吸收新的点：
 *          - 若某点邻居数达到 min_samples，则将其邻居并入当前簇。
 *       4. 重复以上步骤，直到所有点都被标记为某个簇或噪声。
 *
 *       输出为每个点的簇标签，未分配的点标记为噪声。
 *
 *       可以看到DBSCAN需要查询频繁查询一个点的邻居数量，因此我们提前计算好一个点的邻居数量
 *
 *
 * @version 0.1
 * @date 2024-11-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <radar/sp/fixed_point/radar_cluster.h>

#include <radar/common/radar_log.h>
#include <radar/common/mm.h>

#include <stdio.h>


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
 * @brief  查找 idx 号点的邻居
 *
 * @param neighbors 保存邻居的数组
 * @param idx 需要查找的点
 * @param n 量测值的数量
 * @param D 保存量测点之间距离的数组，和radar_calc_meas_distance()函数的D参数相同
 * @param eps DBSCAN的领域大小
 */
static void radar_find_neighbors(size_t *neighbors, size_t idx, size_t n, int32_t *D, int32_t eps)
{
    size_t cnt = 0;
    size_t offset;

    offset = idx * (idx - 1) / 2;
    for (size_t j = 0; j < idx; j++) {
        if (D[offset + j] <= eps) {
            neighbors[cnt++] = j;
        }
    }
    for (size_t j = idx + 1; j < n; j++) {
        if (D[j * (j - 1) / 2 + idx] <= eps) {
            neighbors[cnt++] = j;
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
    memset(n_neighbors, 0, sizeof(size_t) * n);
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
 * @brief  回调函数，返回邻居个数
 *
 * @param[out] count  保存邻居个数的指针
 * @param idx  量测点的序号
 * @param eps  DBSCAN的领域半径
 * @param param  回调函数的额外参数
 * @return  0表示成功
 */
static int radar_get_neighbors_cnt(size_t *count, size_t idx, int32_t eps, void *param)
{
    dbscan_neighbors_t *nb = (dbscan_neighbors_t *)param;
    if (idx >= nb->n) {
        return -1;
    }
    *count = nb->n_neighbors[idx];
    return 0;
}

/**
 * @brief  回调函数，将邻居信息保存在neighbors中
 *
 * @param[out] neighbors  保存邻居的序号的数组
 * @param size  保存邻居的序号的数组大小
 * @param idx  量测点的序号
 * @param eps  DBSCAN的领域半径
 * @param param  回调函数的额外参数
 * @return  0表示成功
 */
static int radar_get_neighbors_info(size_t *neighbors, size_t size, size_t idx, int32_t eps,
                                    void *param)
{
    dbscan_neighbors_t *nb = (dbscan_neighbors_t *)param;
    size_t j = 0;
    size_t n = nb->n;
    size_t cnt;

    if (size < nb->n_neighbors[idx]) {
        return -1;
    }

    radar_find_neighbors(neighbors, idx, n, nb->D, eps);
    return 0;
}

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
                dbscan_get_nbcnt_cb get_n, dbscan_get_nbinfo_cb get_neighbors, void *param)
{
    size_t *stack;
    size_t *neighbors;
    size_t stack_top = 0;
    size_t label_num;

    if (n == 0) {
        return OK;
    }
    if (n == 1) {
        if (min_samples == 0) {
            labels[0] = 0;
            *n_labels = 1;
            return OK;
        } else {
            labels[0] = SIZE_MAX;
            *n_labels = 0;
            return OK;
        }
    }

    stack = (size_t *)rd_malloc((n) * sizeof(size_t));
    if (stack == NULL) {
        RADAR_ERROR("dbscan_core rd_malloc stack failed", RADAR_ENOMEM);
        return -RADAR_ENOMEM;
    }

    neighbors = (size_t *)rd_malloc((n - 1) * sizeof(size_t));
    if (neighbors == NULL) {
        RADAR_ERROR("dbscan_core rd_malloc neighbors failed", RADAR_ENOMEM);
        rd_free(stack);
        return -RADAR_ENOMEM;
    }

    /* 初始化所有点的标签为未访问 */
    memset(labels, 0xFFFF, n * sizeof(labels[0]));

    label_num = 0;
    for (size_t i = 0; i < n; i++) {
        size_t count;

        if (labels[i] != SIZE_MAX) {
            continue;
        }
        get_n(&count, i, eps, param);
        if (count < min_samples)
            continue;

        /* 创建一个新簇 */
        labels[i] = label_num;
        stack[stack_top++] = i;
        /* 通过邻居扩展不断吸收新的点 */
        while (stack_top > 0) {
            size_t j = stack[--stack_top];

            get_n(&count, j, eps, param);
            if (count >= min_samples && !get_neighbors(neighbors, n - 1, j, eps, param)) {
                /* 该点的邻居数量够多且成功获取邻居，进行扩展 */
                for (size_t k = 0; k < count; k++) {
                    size_t v = neighbors[k];
                    if (labels[v] == SIZE_MAX) {
                        stack[stack_top++] = v;
                        labels[v] = label_num;
                    }
                }
            }
        }
        label_num++;
    }
    *n_labels = label_num;

    rd_free(stack);
    rd_free(neighbors);
    return OK;
}

/**
 * @brief  分配 DBSCAN 邻居信息结构
 *
 * @param[in] n 量测点数量
 * @return dbscan_neighbors_t*  Allocated dbscan_neighbors_t (未初始化数据)
 *
 * @note  仅负责内存分配和基础初始化，
 *        使用完成后需调用 radar_cluster_dbscan_neighbors_free() 释放。
 */
dbscan_neighbors_t *radar_cluster_dbscan_neighbors_alloc(size_t n)
{
    dbscan_neighbors_t *nb;
    size_t capacity;

    nb = (dbscan_neighbors_t *)rd_malloc(sizeof(dbscan_neighbors_t));
    if (nb == NULL) {
        RADAR_ERROR("radar_cluster_dbscan_neighbors_alloc rd_malloc dbscan_neighbors_t failed",
                    RADAR_ENOMEM);
        return NULL;
    }

    /* 初始化指针，避免 free 崩溃 */
    nb->D = NULL;
    nb->n_neighbors = NULL;
    nb->capacity = n;

    capacity = n * (n - 1) / 2;

    /* 距离矩阵 */
    nb->D = (int32_t *)rd_malloc(capacity * sizeof(nb->D[0]));
    if (nb->D == NULL) {
        RADAR_ERROR("radar_cluster_dbscan_neighbors_alloc rd_malloc D failed", RADAR_ENOMEM);
        goto errout;
    }

    /* 邻居数量 */
    nb->n_neighbors = (size_t *)rd_calloc(n, sizeof(nb->n_neighbors[0]));
    if (nb->n_neighbors == NULL) {
        RADAR_ERROR("radar_cluster_dbscan_neighbors_alloc rd_malloc n_neighbors failed",
                    RADAR_ENOMEM);
        goto errout;
    }

    return nb;

errout:
    radar_cluster_dbscan_neighbors_free(nb);
    return NULL;
}

/**
 * @brief  初始化 DBSCAN 邻居信息
 *
 * @param[in,out] nb   已分配的邻居结构（容量固定）
 * @param[in]     meas 量测值列表
 * @param[in]     wr   DBSCAN的领域半径
 * @param[in]     wv   DBSCAN的速度半径
 * @param[in]     eps  DBSCAN的邻域阈值
 * @return 0 成功;
 *         -RADAR_ECAPACITY 容量不足（仅部分初始化);
 *         -RADAR_EINVAL 参数无效;
 *
 * @note  如果 meas->num > nb->n，则只处理前 nb->n 个量测点，
 *        并返回 -1。上层应视情况重新分配更大的 nb。
 */
int radar_cluster_dbscan_neighbors_init(dbscan_neighbors_t *nb, measurements_t *meas, int32_t wr,
                                        int32_t wv, int32_t eps)
{
    size_t n_meas;
    size_t n_use;

    if (nb == NULL || meas == NULL) {
        return -RADAR_EINVAL;
    }

    n_meas = meas->num;
    n_use = (n_meas > nb->capacity) ? nb->capacity : n_meas;

    /* 容量不足时舍弃部分量测值 */
    meas->num = n_use;
    nb->n = n_use;
    radar_calc_meas_distance(meas, wr, wv, nb->D);
    radar_calc_neighbors_num(n_use, nb->D, eps, nb->n_neighbors);
    meas->num = n_meas;

    return n_use <= n_meas ? OK : -RADAR_ECAPACITY;
}


/**
 * @brief  创建DBSCAN聚类的邻居信息
 *
 * @param[in] meas 量测值列表
 * @param[in] wr   DBSCAN的领域半径
 * @param[in] wv   DBSCAN的速度半径
 * @param[in] eps  DBSCAN的邻域阈值
 * @return dbscan_neighbors_t*  Created dbscan_neighbors_t
 *
 * @note  该函数会分配内存，需要调用 radar_cluster_dbscan_neighbors_free() 来释放
 */
dbscan_neighbors_t *radar_cluster_dbscan_neighbors_create(measurements_t *meas, int32_t wr,
                                                          int32_t wv, int32_t eps)
{
    dbscan_neighbors_t *nb;
    int n;

    nb = radar_cluster_dbscan_neighbors_alloc(meas->num);
    if (nb == NULL) {
        return NULL;
    }

    radar_cluster_dbscan_neighbors_init(nb, meas, wr, wv, eps);
    return nb;
}

/**
 * @brief 释放 radar_cluster_dbscan_neighbors_create 生成的对象
 *
 * @param nb radar_cluster_dbscan_neighbors_create 生成的对象
 */
void radar_cluster_dbscan_neighbors_free(dbscan_neighbors_t *nb)
{
    if (nb == NULL)
        return;

    if (nb->n_neighbors) {
        rd_free(nb->n_neighbors);
    }

    if (nb->D) {
        rd_free(nb->D);
    }

    rd_free(nb);
}


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
                         size_t min_samples)
{
    int ret;

    RADAR_ASSERT(nb != NULL && labels != NULL && meas != NULL);

    ret = radar_cluster_dbscan_neighbors_init(nb, meas, wr, wv, eps);
    if (ret == OK || ret == -RADAR_ECAPACITY) {
        int status;
        status = dbscan_core(labels, n_labels, nb->n, eps, min_samples, radar_get_neighbors_cnt,
                             radar_get_neighbors_info, nb);

        ret = status == 0 ? ret : status;
    }

    return ret;
}

/**
 * @brief 对DBSCAN聚类结果进行融合，计算每个聚类的均值
 *
 * @param[out] clusters 保存聚类的均值结果
 * @param num_cluster 聚类的数量
 * @param labels 量测值的聚类标签
 * @param meas 量测值列表
 *
 * @return int 0表示成功，-RADAR_ECAPACITY表示clusters容量不足
 */
int radar_cluster_fusion(measurements_t *clusters, size_t num_cluster, size_t *labels,
                         measurements_t *meas)
{
    int ret = 0;
    RADAR_ASSERT(clusters != NULL || num_cluster >= 0);

    if (clusters->capacity < num_cluster) {
        RD_WARN("radar_cluster_fusion clusters->capacity < num_cluster");
        ret = -RADAR_ECAPACITY;
        num_cluster = clusters->capacity;
    }

    for (size_t i = 0; i < num_cluster; i++) {
        measurement_t *m = &clusters->data[i];
        int64_t distance = 0, velocity = 0, azimuth = 0, amp = 0, snr = 0;
        int cnt = 0;

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

        m->amp = amp / cnt;
        m->distance = distance / cnt;
        m->velocity = velocity / cnt;
        m->azimuth = azimuth / cnt;
        m->snr = snr / cnt;
    }

    clusters->num = num_cluster;
    return OK;
}

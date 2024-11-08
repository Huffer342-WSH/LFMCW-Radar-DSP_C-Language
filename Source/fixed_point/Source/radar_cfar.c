/**
 * @file radar_cfar.c
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 2D-CFAR 程序，输入幅度谱输出点云
 * @version 0.1
 * @date 2024-11-05
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "radar_cfar.h"
#include "radar_types.h"
#include <stdlib.h>

int cfar2d_result_reset(cfar2d_result_t *cfar)
{
    cfar->numPoint = 0;
    return 0;
}

/**
 * @brief 分配2D-CFAR检测结果并初始化
 *
 * @param[in] n 需要分配的CFAR检测点的数量
 * @return cfar2d_result_t* 一个指向初始化的2D-CFAR结果结构的指针，或者在失败时为NULL
 */
cfar2d_result_t *cfar2d_result_alloc(size_t n)
{
    // 分配结果结构
    cfar2d_result_t *res = (cfar2d_result_t *)malloc(sizeof(cfar2d_result_t));
    if (res == NULL) {
        return NULL;
    }

    // 分配点云数据
    res->point = (cfar2d_point_t *)malloc(n * sizeof(cfar2d_point_t));
    if (res->point == NULL) {
        free(res);
        return NULL;
    }

    // 初始化
    res->numMax = n;
    res->numPoint = 0;
    res->owner = 1;

    return res;
}

/**
 * @brief 从预分配的内存块中分配和初始化2D-CFAR检测结果
 *
 * @param block 预分配的内存块
 * @param blockSize 预分配的内存块的大小
 * @param n 需要分配的CFAR检测点的数量
 * @return cfar2d_result_t* 一个指向初始化的2D-CFAR结果结构的指针，或者在失败时为NULL
 */
cfar2d_result_t *cfar2d_result_calloc_from_block(void *block, size_t blockSize, size_t n)
{
    // 确保block不为空且blockSize足够
    if (block == NULL || blockSize < n * sizeof(cfar2d_point_t)) {
        return NULL;
    }

    // 分配结果结构
    cfar2d_result_t *result = (cfar2d_result_t *)block;

    // 初始化结果结构
    result->point = (cfar2d_point_t *)(result + 1);
    result->numMax = n;
    result->numPoint = 0;
    result->owner = 0;

    return result;
}

/**
 * @brief 释放2D-CFAR检测结果
 *
 * @param[in] result 2D-CFAR检测结果
 */
void cfar2d_result_free(cfar2d_result_t *result)
{
    if (result == NULL) {
        return;
    }

    // 释放内存
    if (result->owner) {
        free(result->point);
    }
    result->numMax = 0;
    result->point = NULL;
}

/**
 * @brief 2D-CFAR
 *
 * @param[out] res  2D-CFAR检测结果
 * @param[in] mag  输入幅度谱
 * @param[in] cfg  2D-CFAR配置
 * @return 0 - success
 */
int radar_cfar2d_goca(cfar2d_result_t *res, const matrix2d_int32_t *mag, cfar2d_cfg_t *cfg)
{
    return 0;
}

/**
 * @brief 删除部分2D-CFAR检测结果
 *
 * @note CFAR检测结果中，一个目标可能会对应多个点。
 *          但是这些点的幅度角度的旁瓣测角精度较差，
 *          会导致后续聚类时无法正确聚类，
 *          因此要提前删除这些点。
 *
 * @details 当点A和点B的距离小于range，且A < th * B 时，删除点A
 *
 * @param[in] res  CFAR检测结果
 * @param[in] range0  维度0范围
 * @param[in] range1  维度1范围
 * @param[in] th  删除阈值
 * @return 0 - success
 *
 *
 */
int radar_cfar_result_filtering(cfar2d_result_t *res, cfar2d_filter_cfg_t *cfg)
{
    return 0;
}

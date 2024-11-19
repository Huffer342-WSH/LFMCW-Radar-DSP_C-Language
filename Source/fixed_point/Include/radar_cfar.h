/**
 * @file radar_cfar.h
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief CFAR的输入幅度谱，输出检测结果的二维坐标以及信噪比
 * @version 0.1
 * @date 2024-11-20
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "radar_math.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief CFAR检测点
 */
typedef struct {
    uint16_t idx0; ///< 距离维度坐标
    uint16_t idx1; ///< 速度维度坐标
    int32_t amp;   ///< 幅度
    int32_t snr;   ///< 信噪比，Q23.8
} cfar2d_point_t;


/**
 * @brief 2D-CFAR检测结果
 */
typedef struct {
    cfar2d_point_t *point;           ///< 需要外部分配内存然后赋值
    size_t capacity;                 ///< 容量
    size_t numPoint;                 ///< 当前点的个数
    uint8_t is_point_need_free  : 1; ///< point是否需要外部释放
    uint8_t is_struct_need_free : 1; ///< struct是否需要外部释放
    uint8_t reserved            : 6; ///< 保留
} cfar2d_result_t;


/**
 * @brief CFAR配置
 */
typedef struct {
    uint16_t numTrain[2]; ///< 训练单元大小
    uint16_t numGuard[2]; ///< 保护单元大小
    float thSNR;          ///< SNR阈值
    int32_t thAmp;        ///< 幅度阈值
} cfar2d_cfg_t;


/**
 * @brief CFAR结果筛选配置
 */
typedef struct {
    uint16_t range0; ///< 维度0方向上的范围
    uint16_t range1; ///< 维度1方向上的范围
    uint16_t shape1; ///< 维度1方向上的形状
    float th;        ///< 阈值
} cfar2d_filter_cfg_t;


/**
 * @brief 分配2D-CFAR检测结果
 *
 * @param[in] n 结构体能够容纳的CFAR检测点的数量
 * @return cfar2d_result_t* 一个指向初始化的2D-CFAR结果结构的指针，或者在失败时为NULL
 */
cfar2d_result_t *cfar2d_result_alloc(size_t n);


/**
 * @brief 2D-CFAR检测结果的calloc
 *
 * @param[in] block 预分配的内存块
 * @param[in] blockSize 预分配的内存块的大小
 * @param[in] n CFAR检测点的数量
 * @return cfar2d_result_t* 一个指向初始化的2D-CFAR结果结构的指针
 */
cfar2d_result_t *cfar2d_result_calloc_from_block(void *block, size_t blockSize, size_t n);

/**
 * @brief 释放2D-CFAR检测结果
 *
 * @param[in] result 2D-CFAR检测结果
 */
void cfar2d_result_free(cfar2d_result_t *result);


/**
 * @brief CFAR结果中添加一个点
 *
 * @param[in] result CFAR结果
 * @param[in] idx0  point的维度0方向上的索引
 * @param[in] idx1  point的维度1方向上的索引
 * @param[in] amp   point的幅度
 * @param[in] snr   point的信噪比
 * @return 0 - success; 1 - fail
 */
int cfar2d_result_add_point(cfar2d_result_t *result, uint16_t idx0, uint16_t idx1, int32_t amp, int32_t snr);

/**
 * @brief 2D-CFAR检测
 *
 * @param[in] res  CFAR结果
 * @param[in] magSepc2D 幅度谱
 * @param[in] cfg  CFAR配置
 * @return 0 - success; 1 - fail
 */
int radar_cfar2d_goca(cfar2d_result_t *res, const matrix2d_int32_t *magSepc2D, cfar2d_cfg_t *cfg);

/**
 * @brief 2D-CFAR检测（debug version）
 *
 * @param[out] noise  噪声谱
 * @param[in] magSepc2D 幅度谱
 * @param[in] cfg  CFAR配置
 * @return 0 - success; 1 - fail
 */
int radar_cfar2d_goca_debug(matrix2d_int32_t *noise, const matrix2d_int32_t *magSepc2D, cfar2d_cfg_t *cfg);

/**
 * @brief CFAR结果过滤
 *
 * @param[in] res  CFAR结果
 * @param[in] cfg  CFAR结果过滤配置
 * @return 0 - success; 1 - fail
 */
int radar_cfar_result_filtering(cfar2d_result_t *res, const cfar2d_filter_cfg_t *cfg);


#ifdef __cplusplus
}
#endif

/**
 * @file radar_clutter_filter.h
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 提供杂波滤除相关功能
 * @version 0.1
 * @date 2025-09-12
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <radar/sp/fixed_point/radar_math.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    size_t n_channel;         ///< 通道数
    size_t n_rb;              ///< 距离单元数
    uint16_t fifo_len;        ///< FIFO长度
    uint16_t weight_max;      ///< 权重基础
    uint16_t weight_min;      ///< 从弧度映射到权重的增益(Q1.15)
    uint32_t phase_max;       ///< 相位差最大值
    uint32_t phase_min;       ///< 相位差最小值
    uint16_t update_interval; ///< 均值更新间隔
} radar_clutter_filter_cfg_t;

typedef struct {
    int16_t *fifo;       ///< 相位差FIFO动态数组
    int32_t delta_phase; ///< 累积相位差
    int32_t accum_real;  ///< 当前区间累加实部
    int32_t accum_imag;  ///< 当前区间累加虚部
    int16_t prev_phase;  ///< 上一帧相位 Q2.13 [-pi, pi]
    int16_t mean_real;   ///< 平滑后的实部均值 Q1.15
    int16_t mean_imag;   ///< 平滑后的虚部均值 Q1.15
} static_clutter_t;

typedef struct {
    static_clutter_t *static_clutter;
    size_t n_channel;
    size_t n_rb;

    /* 全局控制参数 */
    uint16_t fifo_len;        ///< FIFO长度
    uint16_t weight_max;      ///< 权重最大值(Q1.15)
    uint16_t weight_min;      ///< 权重最小值(Q1.15)
    int32_t weight_gain;      ///< 从弧度映射到权重的增益(Q.20)
    uint32_t phase_min;       ///< 相位差最小值(Q2.13)
    uint16_t update_interval; ///< 均值更新间隔（多少帧更新一次）

    /* 全局计数器（所有单元同步） */
    uint16_t fifo_in;
    uint16_t fifo_count;
    uint16_t frame_count;
} radar_clutter_filter_t;

/**
 * @brief new 一个静态杂波滤波器
 *
 * @param cfg
 * @return radar_clutter_filter_t*
 */
radar_clutter_filter_t *radar_static_clutter_filter_new(const radar_clutter_filter_cfg_t *cfg);

/**
 * @brief delete 一个静态杂波滤波器
 *
 * @param filter
 */
void radar_static_clutter_filter_delete(radar_clutter_filter_t *filter);

/**
 * @brief 静态杂波滤波器。起到快速抑制静态杂波，同时保留微动目标信号的效果
 *
 * @param filter 滤波器结构体
 * @param[inout] rdm RDM矩阵，维度为[通道、距离、速度]
 * @return int
 */
int radar_static_clutter_filter(radar_clutter_filter_t *filter, matrix3d_complex_int16_t *rdm);

#ifdef __cplusplus
}
#endif

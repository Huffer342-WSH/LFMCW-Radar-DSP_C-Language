/**
 * @file track.h
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 目标跟踪功能的C语言接口
 * @version 0.1
 * @date 2024-12-02
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include <radar/common/radar_log.h>
#include <radar/ot/track_target.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    /* 卡尔曼滤波 */
    rd_float_t velocity_noise_coef; ///< 速度噪声系数（用于计算状态转移噪声）
    rd_float_t sigma_phi;           ///< 速度标准差
    rd_float_t sigma_r;             ///< 距离标准差
    rd_float_t sigma_r_dot;         ///< 速度标准差

    /* 数据关联 */
    rd_float_t missed_distance; ///< 无法关联的最大距离

    /* 生命周期维护-航迹起始 */
    rd_float_t time2delete_initiator; ///< 目标关联失败超时时间
    rd_float_t time2init_motion;      ///< 目标是连续运动时，多少时间关联成功
    rd_float_t time2init_static;      ///< 目标是连续静止时，多少时间关联成功
    rd_float_t speed_threshold;       ///< 速度阈值，区分目标是运动还是静止


    /* 生命周期维护-航迹终止 */
    rd_float_t time2delete_deleter;    ///< 目标关联失败超时时间
    rd_float_t time2stop_unassociated; ///< 目标关联失败时，匀减速至0所需时间
    rd_float_t missed_probability;     ///< 目标关联失败概率
    rd_float_t fov[2];                 ///< 角度范围
    rd_float_t radius_range[2];        ///< 距离范围

} tracker_config_t;

typedef struct {
    uint8_t unused;
} tracker_handel_t;

typedef int (*set_meas_callback)(rd_float_t *meas, size_t capacity, void *args);

/**
 * @brief  根据配置创建跟踪器
 *
 * @param   cfg                跟踪器配置
 * @return  tracker_handel_t*  跟踪器句柄，C语言端无法访问
 */
tracker_handel_t *tracker_new(tracker_config_t *cfg);


/**
 * @brief 运行跟踪器
 *
 * @param tracker_handel       跟踪器句柄
 * @param tracked_targets      跟踪状态目标的链表
 * @param unconfirmed_targets  起始状态目标的链表
 * @param measurements         测量值
 * @param timestamp            时间戳
 *
 * @details 输入一帧的测量值，内部完成已有目标的匹配和更新，以及新目标的创建等
 *          修改后的结果依然保存在tracked_targets和unconfirmed_targets中
 */
// void tracker_run(tracker_handel_t *tracker_handel, tracked_targets_list_t *tracked_targets, tracked_targets_list_t *unconfirmed_targets,
//                  measurements_t *measurements, uint32_t timestamp_ms);

void tracker_run(tracker_handel_t *tracker_handel, tracked_targets_list_t *tracked_targets, tracked_targets_list_t *unconfirmed_targets, size_t meas_num,
                 uint32_t timestamp_ms, set_meas_callback cb, void *args);
#ifdef __cplusplus
}
#endif

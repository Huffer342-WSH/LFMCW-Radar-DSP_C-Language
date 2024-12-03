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

#include "track_target.h"
#include "radar_measurement.h"

#include "radar_log.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double velocity_noise_coef;
    double sigma_phi;
    double sigma_r;
    double sigma_r_dot;
    double missed_distance;
} tracker_config_t;

typedef struct {
} tracker_handel_t;


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
void tracker_run(tracker_handel_t *tracker_handel, tracked_targets_list_t *tracked_targets, tracked_targets_list_t *unconfirmed_targets,
                 measurements_t *measurements, uint32_t timestamp_ms);


#ifdef __cplusplus
}
#endif

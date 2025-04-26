/**
 * @file track_tracking.hh
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 目标跟踪的C++接口
 * @version 0.1
 * @date 2024-12-02
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include <radar/common/radar_math_types.h>


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

#ifdef __cplusplus
}

/* C++ API */

#include <radar/ot/track_target.hh>
#include <radar/ot/track_associator.hh>
#include <radar/ot/track_deleter.hh>
#include <radar/ot/track_initiator.hh>
#include <radar/ot/track_kalman.hh>
#include <radar/ot/track_model.hh>


class Tracker
{
public:
    KalmanUpdater updater;
    KalmanPredictor predictor;
    Associator associator;
    Deleter deleter;
    Initiator initiator;

    std::vector<Vector3r> measurements;


    Tracker(tracker_config_t *cfg)
        : updater(cfg->sigma_phi, cfg->sigma_r, cfg->sigma_r_dot)
        , predictor(cfg->velocity_noise_coef)
        , associator(predictor, updater, cfg->missed_distance)
        , initiator(associator, cfg->time2delete_initiator, cfg->time2init_motion, cfg->time2init_static, cfg->speed_threshold, cfg->missed_distance)
        , deleter(cfg->time2delete_deleter, cfg->missed_probability, cfg->time2stop_unassociated, cfg->fov, cfg->radius_range)
    {
    }

    void track(TrackedTargets &tracked_targets, TrackedTargets &unconfirmed_targets, std::vector<Vector3r> &measurements, uint32_t timestamp_ms);
};

#endif

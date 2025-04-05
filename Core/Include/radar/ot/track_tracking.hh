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
    rd_float_t velocity_noise_coef;
    rd_float_t sigma_phi;
    rd_float_t sigma_r;
    rd_float_t sigma_r_dot;
    rd_float_t missed_distance;
    rd_float_t unassociated_time; // 目标关联失败超时时间
    rd_float_t keep_motion_time;  // 目标是连续运动时，多少时间关联成功
    rd_float_t keep_static_time;  // 目标是连续静止时，多少时间关联成功
    rd_float_t speed_threshold;   // 速度阈值


    rd_float_t missed_probability;

    rd_float_t fov[2];
    rd_float_t radius_range[2];
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
        , deleter(cfg->unassociated_time, cfg->missed_probability, cfg->fov, cfg->radius_range)
        , initiator(associator, cfg->unassociated_time, cfg->keep_motion_time, cfg->keep_static_time, cfg->speed_threshold, cfg->missed_distance)
    {
    }

    void track(TrackedTargets &tracked_targets, TrackedTargets &unconfirmed_targets, std::vector<Vector3r> &measurements, uint32_t timestamp_ms);
};

#endif

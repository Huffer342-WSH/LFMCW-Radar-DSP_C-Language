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

#include <radar/ot/track.h>

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

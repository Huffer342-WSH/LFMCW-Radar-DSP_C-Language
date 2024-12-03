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

#include "track.h"

#include "track_target.hh"
#include "track_associator.hh"
#include "track_deleter.hh"
#include "track_initiator.hh"
#include "track_kalman.hh"
#include "track_model.hh"


#ifdef __cplusplus


class Tracker
{

public:
    KalmanUpdater updater;
    KalmanPredictor predictor;
    Associator associator;
    Deleter deleter;
    Initiator initiator;

    Tracker(tracker_config_t *cfg)
        : updater(cfg->sigma_phi, cfg->sigma_r, cfg->sigma_r_dot)
        , predictor(cfg->velocity_noise_coef)
        , associator(predictor, updater, cfg->missed_distance)
        , deleter()
        , initiator(associator)
    {
    }

    void track(TrackedTargets &tracked_targets, TrackedTargets &unconfirmed_targets, std::vector<Eigen::Vector3d> &measurements, uint32_t timestamp_ms);
};


extern "C" {
#endif


#ifdef __cplusplus
}
#endif

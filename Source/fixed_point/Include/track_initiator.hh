#pragma once

#include "radar_math.h"
#include "track_associator.hh"
#include "track_kalman.hh"
#include "track_target.hh"

#include <vector>

class Initiator
{
private:
public:
    static const int32_t initial_score = 1000;
    static const int32_t max_score = 3000;

    Associator &associator;
    rd_float_t unassociated_time; // 目标关联失败超时时间
    rd_float_t keep_motion_time;  // 目标是连续运动时，多少时间关联成功
    rd_float_t keep_static_time;  // 目标是连续静止时，多少时间关联成功
    rd_float_t speed_threshold;   // 速度阈值
    rd_float_t missed_distance;
    Eigen::Matrix<double, 4, 4> init_covar = Eigen::Matrix<double, 4, 4>::Zero();
    

    int32_t unassociated_score;
    int32_t motion_score;
    int32_t static_score;

    Initiator(Associator &associator, rd_float_t unassociated_time, rd_float_t keep_motion_time, rd_float_t keep_static_time, rd_float_t speed_threshold,
              rd_float_t missed_distance)
        : associator(associator)
        , unassociated_time(unassociated_time)
        , keep_motion_time(keep_motion_time)
        , keep_static_time(keep_static_time)
        , speed_threshold(speed_threshold)
        , missed_distance(missed_distance)
    {
        unassociated_score = (int32_t)(-this->initial_score / this->unassociated_time);
        int32_t score = this->max_score - this->initial_score; // 不用当成员变量，写在构造函数里，这种算出来得到的变量
        motion_score = (int32_t)(score / this->keep_motion_time);
        static_score = (int32_t)(score / this->keep_static_time);
        for (int i = 0; i < 4; ++i) {
            init_covar(i, i) = 0.1 * 0.1;
        }

        RD_DEBUG("unassociated_time: %f\nkeep_motion_time: %f\nkeep_static_time: %f\nspeed_threshold: %f\nmissed_distance: %f\n"
                 "unassociated_score: %d\nmotion_score: %d\nstatic_score: %d\n",
                 unassociated_time, keep_motion_time, keep_static_time, speed_threshold, missed_distance, unassociated_score, motion_score, static_score);

        return;
    };

    ~Initiator() { };

    void initiate(TrackedTargets &tracked_targets, TrackedTargets &unconfirmed_targets, std::vector<Vector3r> &measurements, uint32_t timestamp_ms);

    void update_lifecycle(TrackedTargets &tracked_targets, std::vector<Hypothesis> &hypotheses);

    void move_confirmed_targets(TrackedTargets &tracked_targets, TrackedTargets &unconfirmed_targets);

    void creat_targets(TrackedTargets &unconfirmed_targets, std::vector<Vector3r> &measurements, uint32_t timestamp_ms);
};

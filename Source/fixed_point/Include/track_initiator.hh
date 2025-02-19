#pragma once

#include "track_target.hh"
#include "track_associator.hh"
#include "track_kalman.hh"
#include "radar_math.h"

#include <vector>

class Initiator
{
private:
public:
    
    Associator &associator;
    rd_float_t unassociated_time;//目标关联失败超时时间
    rd_float_t keep_motion_time;//目标是连续运动时，多少时间关联成功
    rd_float_t keep_static_time;// 目标是连续静止时，多少时间关联成功
    rd_float_t speed_threshold;// 速度阈值
    rd_float_t missed_distance;
    uint32_t initial_score;
    uint32_t max_score;

    uint32_t unassociated_score = uint32_t(-this->initial_score / this->unassociated_time);
    uint32_t score = this->max_score - this->initial_score;
    uint32_t motion_score = uint32_t(this->score / this->keep_motion_time);
    uint32_t static_score = uint32_t(this->score / this->keep_static_time);
    Initiator(Associator &associator)
        : associator(associator)
        , unassociated_time(2.0f)
        , keep_motion_time(2.0f)
        , keep_static_time(8.0f)
        , speed_threshold(0.1f)
        , missed_distance(5.0f)
        , initial_score(1000)
        , max_score(3000)
    {
        return;
    };

    ~Initiator() { };

    void initiate(TrackedTargets &tracked_targets, TrackedTargets &unconfirmed_targets, std::vector<Vector3r> &measurements, uint32_t timestamp_ms);

    void update_lifecycle(std::vector<Hypothesis> &hypotheses,LifeCycle &life_cycle);


    void move_confirmed_targets(TrackedTargets &tracked_targets, TrackedTargets &unconfirmed_targets);


    void creat_targets(TrackedTargets &unconfirmed_targets, std::vector<Vector3r> &measurements, uint32_t timestamp_ms);
};

#pragma once

#include "track_kalman.hh"
#include "track_target.hh"

#include <vector>

class Deleter
{
private:
public:
    static const int max_score = 5000;

    rd_float_t unassociated_time;
    rd_float_t missed_probability;
    int32_t unassociated_score;
    std::array<rd_float_t, 2> fov;
    std::array<rd_float_t, 2> radius_range;
    Deleter(rd_float_t unassociated_time, rd_float_t missed_probability, std::array<rd_float_t, 2> fov, std::array<rd_float_t, 2> radius_range) 
    : unassociated_time(unassociated_time)
    , missed_probability(missed_probability)
    , fov(fov)
    , radius_range(radius_range)
    {
        unassociated_score = -this->max_score / this->unassociated_time;
     };

    ~Deleter() { };

    void update_lifecycle(TrackedTargets &tracked_targets, std::vector<Hypothesis> &hypotheses);

    void delete_tracks(TrackedTargets &tracked_targets, std::vector<Hypothesis> &hypotheses);
};

#pragma once

#include <radar/ot/track_kalman.hh>
#include <radar/ot/track_target.hh>
#include <radar/ot/track_kalman.hh>

#include <vector>
#include <Eigen/Dense>

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
    Deleter(rd_float_t unassociated_time, rd_float_t missed_probability, rd_float_t fov[2], rd_float_t radius_range[2])
        : unassociated_time(unassociated_time)
        , missed_probability(missed_probability)
    {
        memcpy(this->fov.data(), fov, this->fov.size() * sizeof(rd_float_t));
        memcpy(this->radius_range.data(), radius_range, sizeof(rd_float_t) * this->radius_range.size());
        unassociated_score = -this->max_score / this->unassociated_time;
     };

    ~Deleter() { };

    void update_lifecycle(TrackedTargets &tracked_targets, std::vector<Hypothesis> &hypotheses);

    void delete_tracks(TrackedTargets &tracked_targets, std::vector<Hypothesis> &hypotheses);
};

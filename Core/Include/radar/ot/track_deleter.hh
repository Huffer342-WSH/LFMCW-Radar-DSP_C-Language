#pragma once

#include <radar/lib/PiecewiseLinear.hh>
#include <radar/ot/track_kalman.hh>
#include <radar/ot/track_target.hh>

#include <vector>
#include <Eigen/Dense>

class Deleter
{
private:
public:
    static const int max_score = 5000;

    PiecewiseLinear<rd_float_t> gain_func; ///< 增益函数
    rd_float_t unassociated_time;
    rd_float_t missed_probability;
    int32_t unassociated_score;
    rd_float_t time2stop_unassociated = INFINITY; ///< 目标关联失败时，匀减速至0所需时间

    std::array<rd_float_t, 2> fov;
    std::array<rd_float_t, 2> radius_range;
    Deleter(rd_float_t unassociated_time, rd_float_t missed_probability,
            rd_float_t time2stop_unassociated, rd_float_t fov[2], rd_float_t radius_range[2])
        : gain_func(0.05, 1.1, 0.1, -0.1)
        , unassociated_time(unassociated_time)
        , missed_probability(missed_probability)
        , time2stop_unassociated(time2stop_unassociated)
    {
        memcpy(this->fov.data(), fov, this->fov.size() * sizeof(rd_float_t));
        memcpy(this->radius_range.data(), radius_range, sizeof(rd_float_t) * this->radius_range.size());
        unassociated_score = -this->max_score / this->unassociated_time;
    };

    ~Deleter() { };

    void delete_tracks(TrackedTargets &tracked_targets, std::vector<Hypothesis> &hypotheses);

    void update_score(TrackedTarget &target, Hypothesis &hypothesis);

    void update_score(TrackedTargets &tracked_targets, std::vector<Hypothesis> &hypotheses);

    void update_state(TrackedTarget &target, Hypothesis &hypothesis);
};

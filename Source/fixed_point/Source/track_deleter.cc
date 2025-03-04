// Deleter.cpp
#include "track_deleter.hh"


void Deleter::delete_tracks(TrackedTargets &tracked_targets, std::vector<Hypothesis> &hypotheses)
{

    update_lifecycle(tracked_targets, hypotheses);

    tracked_targets.delete_invalid_targets();

    return;
}

void Deleter::update_lifecycle(TrackedTargets &tracked_targets, std::vector<Hypothesis> &hypotheses)
{

#if LOG_LEVEL <= 5
#define LLOG(format, ...) RADAR_LOG_PRINTF(format, ##__VA_ARGS__)
#else
#define LLOG(format, ...)
#endif

    std::vector<Hypothesis>::iterator h = hypotheses.begin();

    for (TrackedTarget &target : tracked_targets) {

        LifeCycle &l = target.life_cycle;
        rd_float_t dt = (rd_float_t)(h->prediction.timestamp_ms - h->prior_state.timestamp_ms) / 1000; // 计算时间戳差的秒数

        LLOG("UUID: %d score:%d\n", target.uuid, target.life_cycle.score);
        LLOG("dt:%f  has_meas: %s\n", dt, h->has_meas ? "yes" : "no");

        int32_t score = 0;
        if (!(h->has_meas)) {
            l.unassociated_time += dt;
            score += int32_t(this->unassociated_score * dt); // 加上未关联时间乘时间差
        } else {
            score -= this->unassociated_score * dt * this->missed_probability / (1 - this->missed_probability);
            l.unassociated_time = 0; // 重置未关联时间
        }

        l.score += score;

        rd_float_t angle = std::atan2(target.state.state_vector[2], target.state.state_vector[0]); // 计算角度
        Eigen::Vector2d sub_vector(target.state.state_vector[0], target.state.state_vector[2]);
        rd_float_t r = sub_vector.norm();
        LLOG("angle:%f [%f %f] r: %f[%f %f]\n", angle, this->fov[0], this->fov[1], r, this->radius_range[0], this->radius_range[1]);
        if (angle < this->fov[0] || angle > this->fov[1] || r > this->radius_range[1] || r < this->radius_range[0]) {
            l.score = -1;
        }

        if (l.score > this->max_score) {
            l.score = this->max_score;
        }

        LLOG("score:%d\n", target.life_cycle.score);
        h++;
    }
}

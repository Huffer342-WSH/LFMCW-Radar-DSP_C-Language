// Deleter.cpp
#include <radar/ot/track_deleter.hh>


void Deleter::delete_tracks(TrackedTargets &tracked_targets, std::vector<Hypothesis> &hypotheses)
{

    update_lifecycle(tracked_targets, hypotheses);

    tracked_targets.delete_invalid_targets();

    return;
}

void Deleter::update_lifecycle(TrackedTargets &tracked_targets, std::vector<Hypothesis> &hypotheses)
{
#undef FORCE_ENABLE_LOG
#define FORCE_ENABLE_LOG 0

    RD_DEBUG("[航迹删除管理——更新生命周期]");
#define LOG_NO_PREFIX
    std::vector<Hypothesis>::iterator h = hypotheses.begin();

    for (TrackedTarget &target : tracked_targets) {

        LifeCycle &l = target.life_cycle;
        rd_float_t dt = (rd_float_t)(h->prediction.timestamp_ms - h->prior_state.timestamp_ms) / 1000; // 计算时间戳差的秒数

        RD_DEBUG("UUID: %d score:%d\n", target.uuid, target.life_cycle.score);
        RD_DEBUG("dt:%f  has_meas: %s\n", dt, h->has_meas ? "yes" : "no");

        int32_t score = 0;
        if (!(h->has_meas)) {
            l.unassociated_time += dt;
            score += int32_t(this->unassociated_score * dt); // 加上未关联时间乘时间差
        } else {
            score -= l.unassociated_time * this->unassociated_score * this->missed_probability / (1 - this->missed_probability);
            score -= static_cast<int32_t>(dt * this->unassociated_score);
            l.unassociated_time = 0; // 重置未关联时间
        }

        l.score += score;

        rd_float_t angle = std::atan2(target.state.state_vector[2], target.state.state_vector[0]); // 计算角度
        Eigen::Vector2d sub_vector(target.state.state_vector[0], target.state.state_vector[2]);
        rd_float_t r = sub_vector.norm();
        RD_DEBUG("angle:%f [%f %f] r: %f[%f %f]\n", angle, this->fov[0], this->fov[1], r, this->radius_range[0], this->radius_range[1]);
        if (angle < this->fov[0] || angle > this->fov[1] || r > this->radius_range[1] || r < this->radius_range[0]) {
            l.score = -1;
        }

        if (l.score > this->max_score) {
            l.score = this->max_score;
        }

        RD_DEBUG("score:%d\n\n", target.life_cycle.score);
        h++;
    }

#undef FORCE_ENABLE_LOG
#define FORCE_ENABLE_LOG 0
}

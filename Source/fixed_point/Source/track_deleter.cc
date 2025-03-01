// Deleter.cpp
#include "track_deleter.hh"

void Deleter::delete_invalid_targets(TrackedTargets &tracked_targets)
{
    for (TrackedTargets::iterator it = tracked_targets.begin(); it != tracked_targets.end();) {
        if ((*it).life_cycle.score <= 0) {
            it = tracked_targets.erase(it);
        } else {
            it++;
        }
    }
}
void Deleter::delete_tracks(TrackedTargets &tracked_targets, std::vector<Hypothesis> &hypotheses)
{

    update_lifecycle(tracked_targets, hypotheses);

    delete_invalid_targets(tracked_targets);

    return;
}

void Deleter::update_lifecycle(TrackedTargets &tracked_targets, std::vector<Hypothesis> &hypotheses)
{

    std::vector<Hypothesis>::iterator h = hypotheses.begin();

    for (TrackedTarget &target : tracked_targets) {
        int32_t score = 0;
        LifeCycle &l = target.life_cycle;
        RD_DEBUG("score:%d\n", score);
        RD_DEBUG("lifecycle_score:%d\n", l.score);
        rd_float_t dt = (h->prediction.timestamp_ms - h->prior_state.timestamp_ms) / 1000; // 计算时间戳差的秒数
        RD_DEBUG("dt:%f\n", dt);
        RD_DEBUG("unassociated_score:%d\n", this->unassociated_score);

        if (!(h->has_meas)) {

            l.unassociated_time += dt;
            score += int32_t(this->unassociated_score * dt); // 加上未关联时间乘时间差

            RD_DEBUG("score:%d\n", score);
        } else {
            RD_DEBUG("enter associated\n");
            score -= this->unassociated_score * dt * this->missed_probability / (1 - this->missed_probability);
            RD_DEBUG("score:%d\n", score);
            l.unassociated_time = 0; // 重置未关联时间
        }

        l.score += score;
        RD_DEBUG("score:%d\n", score);
        RD_DEBUG("lifecycle_score:%d\n", l.score);
        RD_DEBUG("---------------------------------------------");
        rd_float_t angle = std::atan2(target.state.state_vector[2], target.state.state_vector[0]); // 计算角度
        RD_DEBUG("angle:%f\n", angle);
        Eigen::Vector2d sub_vector(target.state.state_vector[0], target.state.state_vector[2]);
        rd_float_t r = sub_vector.norm();
        RD_DEBUG("r:%f\n", r);
        // 离开范围得删除
        if (angle < this->fov[0] || angle > this->fov[1] || r > this->radius_range[1] || r < this->radius_range[0]) {
            l.score = -1;
        }
        if (l.score > this->max_score) {
            l.score = this->max_score;
        }
        RD_DEBUG("lifecycle_score:%d\n", l.score);
        h++;
    }
}
#include <radar/ot/track_deleter.hh>

void Deleter::delete_tracks(TrackedTargets &tracked_targets, std::vector<Hypothesis> &hypotheses)
{
    RADAR_ASSERT_EQ((size_t)tracked_targets.size(), (size_t)hypotheses.size()); // 检查目标数和假设数是否匹配

    std::vector<Hypothesis>::iterator h = hypotheses.begin();
    for (TrackedTarget &target : tracked_targets) {

        update_score(target, *h);

        update_state(target, *h);
        h++;
    }
    tracked_targets.delete_invalid_targets();
    return;
}

void Deleter::update_score(TrackedTarget &target, Hypothesis &hypothesis)
{
#undef FORCE_ENABLE_LOG
#define FORCE_ENABLE_LOG 0

    RD_DEBUG("[航迹删除管理——更新生命周期]");
#define LOG_NO_PREFIX


    LifeCycle &l = target.life_cycle;
    rd_float_t dt = (rd_float_t)(hypothesis.prediction.timestamp_ms - hypothesis.prior_state.timestamp_ms) / 1000; // 计算时间戳差的秒数

    RD_DEBUG("UUID: %d score:%d\n", target.uuid, target.life_cycle.score);
    RD_DEBUG("dt:%f  has_meas: %s\n", dt, hypothesis.has_meas ? "yes" : "no");

    /* 根据关联结果更新生命周期 */
    int32_t score = 0;
    if (!(hypothesis.has_meas)) {
        l.unassociated_time += dt;
        score += int32_t(this->unassociated_score * dt); // 加上未关联时间乘时间差
    } else {
        score -= l.unassociated_time * this->unassociated_score * this->missed_probability / (1 - this->missed_probability);
        score -= static_cast<int32_t>(dt * this->unassociated_score);
        l.unassociated_time = 0; // 重置未关联时间
    }
    l.score += score;

    /* 检查目标是否在范围内 */
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


#undef FORCE_ENABLE_LOG
#define FORCE_ENABLE_LOG 0
}


void Deleter::update_score(TrackedTargets &tracked_targets, std::vector<Hypothesis> &hypotheses)
{
    std::vector<Hypothesis>::iterator h = hypotheses.begin();
    for (TrackedTarget &target : tracked_targets) {
        update_score(target, *h);
        h++;
    }
}


void Deleter::update_state(TrackedTarget &target, Hypothesis &hypothesis)
{
    auto calc_v = [](rd_float_t v0, rd_float_t T, rd_float_t t) {
        rd_float_t ans = 0;
        if (T == INFINITY || t < 1e-6) {
            ans = v0;
        } else if (t < T && T > 1e-6) {
            ans = v0 * (T - t) / T;
        } else {
            ans = 0;
        }
        return ans;
    };

    rd_float_t t = target.life_cycle.unassociated_time;

    if (t > 1e-6) {
        GaussianState &state = target.state;
        rd_float_t T = this->time2stop_unassociated;

        state.covar *= 2;
        state.state_vector(1) = calc_v(target.life_cycle.state_prev.state_vector(1), T, t);
        state.state_vector(3) = calc_v(target.life_cycle.state_prev.state_vector(3), T, t);
    }
}

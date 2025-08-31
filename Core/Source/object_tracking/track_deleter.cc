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
    LifeCycle &l = target.life_cycle;
    int32_t score = 0;
    rd_float_t dt = l.timestep;
    rd_float_t angle = target.meas_post(0);
    rd_float_t r = target.meas_post(1);
    rd_float_t r_err;

    RD_DEBUG("[航迹删除管理——更新生命周期]");

    /* 根据关联结果更新生命周期 */
    if (!(hypothesis.has_meas)) {
        l.unassociated_time += dt;
        score += (int32_t)(unassociated_score * dt);

        RD_DEBUG("目标[%d] 关联失败 dt=%.2f utime=%.2f score=%d\n", target.uuid, dt,
                 l.unassociated_time, score);
    } else {
        rd_float_t gain;

        r_err = abs(l.meas_prederr(1));

        score -= l.unassociated_time * this->unassociated_score * this->missed_probability /
            (1 - this->missed_probability);
        score -= static_cast<int32_t>(dt * this->unassociated_score);

        // 预测的准获得额外增益
        gain = gain_func(r_err);
        score = (int32_t)((rd_float_t)score * gain);

        // 重置未关联时间
        l.unassociated_time = 0;

        RD_DEBUG("目标[%d] 关联成功 dt=%.2f r_err=%.2f utime=%.2f score=%d\n", target.uuid, dt,
                 r_err, l.unassociated_time, score);
    }
    l.score += score;

    /* 超出roi删除 */
    if (angle < this->fov[0] || angle > this->fov[1] || r > this->radius_range[1] ||
        r < this->radius_range[0]) {
        l.score = -1;
    }

    if (l.score > this->max_score) {
        l.score = this->max_score;
    }
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


    if (!hypothesis.has_meas) {
        rd_float_t T = this->time2stop_unassociated;
        rd_float_t t = target.life_cycle.unassociated_time;
        Vector4r &x = target.state.state_vector;
        Matrix44r &covar = target.state.covar;

        covar *= 2;
        x(1) = calc_v(x(1), T, t);
        x(3) = calc_v(x(3), T, t);
    }
}

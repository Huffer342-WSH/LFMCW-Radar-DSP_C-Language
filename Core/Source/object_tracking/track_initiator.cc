#include <radar/ot/track_initiator.hh>
#include <radar/ot/track_kalman.hh>
#include <radar/common/radar_log.h>

void Initiator::initiate(TrackedTargets &tracked_targets, TrackedTargets &unconfirmed_targets, std::vector<Vector3r> &measurements, uint32_t timestamp_ms)
{
    RD_DEBUG("[航迹起始]——开始");
    std::vector<Hypothesis> hypotheses; // 在子代码块中的局部变量会在代码块结束后被销毁
    this->associator.hypotheses_init(hypotheses, unconfirmed_targets);

    /* 数据关联 */
    this->associator.associate(hypotheses, measurements, timestamp_ms);


    /* 滤波：更新器根据假设计算后验状态，并赋值给目标 */
    this->associator.update(unconfirmed_targets, hypotheses);


    /*  更新生命周期 */
    this->update_score(unconfirmed_targets, hypotheses);


    /* 删除无效目标 */
    unconfirmed_targets.delete_invalid_targets();


    /* 将起始成功的目标移动到已跟踪目标列表 */
    this->move_confirmed_targets(tracked_targets, unconfirmed_targets);


    /*  从仍没有被关联的测量值中选取可能的值生成新跟踪 */
    this->creat_targets(unconfirmed_targets, measurements, timestamp_ms);

    RD_DEBUG("[航迹起始]——结束");
    return;
}


void Initiator::update_score(TrackedTargets &tracked_targets, std::vector<Hypothesis> &hypotheses)
{
    RD_DEBUG("[航迹起始]-更新生命周期 开始");

    std::vector<Hypothesis>::iterator h = hypotheses.begin();
    for (TrackedTarget &target : tracked_targets) {
        LifeCycle &l = target.life_cycle;
        Vector3r &meas_in = h->measurement;
        Vector3r &meas_pre = h->measurement_prediction.state_vector;
        int32_t score = 0;
        rd_float_t speed = 0;
        rd_float_t dt = l.timestep;
        rd_float_t r_err;

        if (!(h->has_meas)) {
            // 关联失败，超时了，扣除一半分数
            if (l.unassociated_time > this->unassociated_time) {
                score -= l.score / 2;
            }
            score += int32_t(this->unassociated_score * dt); // 加上未关联时间乘时间差

            l.unassociated_time += dt;
        } else {
            rd_float_t gain; ///< 加分增益，预测和测量之间的误差越小，增益越高
            Vector4r &x = target.state.state_vector;

            r_err = abs(l.meas_prederr(1));
            speed = hypot(x(1), x(3));

            // 加分
            if (speed > this->speed_threshold) {
                score += (int32_t)(this->motion_score * dt);
            } else {
                score += (int32_t)(this->static_score * dt);
            }

            // 预测的准获得额外增益
            gain = gain_func(r_err);
            score = (int32_t)((rd_float_t)score * gain);

            // 返还一部分因为关联失败扣掉的分数
            score -= l.unassociated_time * this->unassociated_score / 2;

            l.unassociated_time = 0;
        }

        RD_DEBUG("目标%d 关联[%c] dt=%f err=%f speed=%f score=%d\n", target.uuid,
                 (h->has_meas) ? 'Y' : 'N', dt, r_err, speed, score);

        l.score += score;
        h++;
    }
    RD_DEBUG("[航迹起始]-更新生命周期 结束");
}

void Initiator::move_confirmed_targets(TrackedTargets &tracked_targets, TrackedTargets &unconfirmed_targets)
{
    RD_DEBUG("[目标跟踪-航迹起始管理]");
    TrackedTargets::iterator it = unconfirmed_targets.begin(); // 遍历
    while (it != unconfirmed_targets.end()) {
        if (it->life_cycle.score >= this->max_score) { // 判断是否得分合规
            RD_DEBUG("目标%d  %d分 航迹起始成功", it->uuid, it->life_cycle.score);
            TrackedTargets::iterator temp = it;
            it++;
            tracked_targets.splice(tracked_targets.begin(), unconfirmed_targets, temp);

        } else {
            RD_DEBUG("目标%d  %d分 分数不足", it->uuid, it->life_cycle.score);
            it++;
        }
    }
    return;
}


void Initiator::creat_targets(TrackedTargets &unconfirmed_targets, std::vector<Vector3r> &measurements, uint32_t timestamp_ms)
{
    RD_DEBUG("[目标跟踪-新建目标]");
    speed_threshold = 0.1; // 速度阈值  太慢的速度不要
    // 仍然没有被关联的测量值，用于创建新目标
    // 遍历未关联的测量值

    for (const auto &measurement : measurements) {
        // 检查测量值的速度是否超过阈值
        RD_DEBUG("待添加测量值速度:%f\n", measurement(2));
        if (std::abs(measurement(2)) > speed_threshold) {
            // 如果超过阈值，创建新的跟踪目标并添加到未确认目标列表中
            unconfirmed_targets.emplace_back(measurement, this->init_covar, timestamp_ms);
            auto lastIt = unconfirmed_targets.rbegin();
            lastIt->life_cycle.score = this->initial_score;
            RD_DEBUG("新建目标速度:%f\n", measurement(2));
        }
    }
    return;
}

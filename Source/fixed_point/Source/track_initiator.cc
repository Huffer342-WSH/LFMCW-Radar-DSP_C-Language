#include "track_initiator.hh"

#include "track_kalman.hh"

#include "radar_log.h"
#include <Eigen/Dense>


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
    this->update_lifecycle(unconfirmed_targets, hypotheses);


    /* 将起始成功的目标移动到已跟踪目标列表 */
    this->move_confirmed_targets(tracked_targets, unconfirmed_targets);


    /*  从仍没有被关联的测量值中选取可能的值生成新跟踪 */
    this->creat_targets(unconfirmed_targets, measurements, timestamp_ms);

    RD_DEBUG("[航迹起始]——结束");
    return;
}


void Initiator::update_lifecycle(TrackedTargets &tracked_targets, std::vector<Hypothesis> &hypotheses)
{
    RD_DEBUG("[航迹起始]-更新生命周期 开始");
#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define LLOG(format, ...) RADAR_LOG_PRINTF(format, ##__VA_ARGS__)
#else
#define LLOG(format, ...)
#endif

    std::vector<Hypothesis>::iterator h = hypotheses.begin();


    for (TrackedTarget &target : tracked_targets) {
        int32_t score = 0;
        rd_float_t speed = 0;
        LifeCycle &l = target.life_cycle;
        LLOG("\n目标%d:\n", target.uuid);
        LLOG("score:%d\n", score);
        LLOG("lifecycle_score:%d\n", l.score);
        LLOG("unassociated_time:%f\n", l.unassociated_time);
        rd_float_t dt = (rd_float_t)(h->prediction.timestamp_ms - h->prior_state.timestamp_ms) / 1000; // 计算时间戳差的秒数
        LLOG("时间戳 %u %u dt:%f\n", h->prediction.timestamp_ms, h->prior_state.timestamp_ms, dt);

        if (!(h->has_meas)) {
            // 关联失败，超时了，扣除一半分数
            if (l.unassociated_time > this->unassociated_time) {
                LLOG("enter unassociated\n");
                score -= l.score / 2; // 改用有符号数表示score，不用右移代替除以2

                LLOG("score:%d\n", score);
                LLOG("unassociated_time:%f\n", l.unassociated_time);
            }
            score += int32_t(this->unassociated_score * dt); // 加上未关联时间乘时间差
            LLOG("unassociated_score:%d\n", this->unassociated_score);
            l.unassociated_time += dt;
            LLOG("score:%d\n", score);
            LLOG("unassociated_time:%f\n", l.unassociated_time);
        } else {
            LLOG("enter associated\n");
            score -= l.unassociated_time * this->unassociated_score / 2;
            LLOG("score:%d\n", score);
            speed = std::abs(h->measurement(2));
            LLOG("测量值：[%f %f %f]\n", h->measurement(0), h->measurement(1), h->measurement(2));
            LLOG("速度 %f  阈值%f\n", speed, this->speed_threshold);
            if (speed > this->speed_threshold) {
                LLOG("enter move\n");
                score += (int32_t)(this->motion_score * dt);
                LLOG("motion_score:%d\n", this->motion_score);
                LLOG("score:%d\n", score);
            } else {
                LLOG("enter static\n");
                score += (int32_t)(this->static_score * dt);
                LLOG("static_score:%d\n", this->static_score);
                LLOG("score:%d\n", score);
            }
            l.unassociated_time = 0;
        }
        l.score += score;
        LLOG("score:%d\n", score);
        LLOG("lifecycle_score:%d\n", l.score);
        LLOG("unassociated_time:%f\n", l.unassociated_time);

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

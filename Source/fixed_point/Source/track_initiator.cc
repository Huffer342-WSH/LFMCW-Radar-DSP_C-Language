#include "track_initiator.hh"

#include "track_kalman.hh"


void Initiator::initiate(TrackedTargets &tracked_targets, TrackedTargets &unconfirmed_targets, std::vector<Vector3r> &measurements, uint32_t timestamp_ms)
{

    std::vector<Hypothesis> hypotheses; // 在子代码块中的局部变量会在代码块结束后被销毁
    this->associator.hypotheses_init(hypotheses, unconfirmed_targets);

    /* 数据关联 */
    RD_DEBUG("[目标跟踪] 数据关联");
    this->associator.associate(hypotheses, measurements, timestamp_ms);


    /* 滤波：更新器根据假设计算后验状态，并赋值给目标 */
    RD_DEBUG("[目标跟踪] 滤波");
    this->associator.update(unconfirmed_targets, hypotheses);


    /*  更新生命周期 */
    //this->update_lifecycle(unconfirmed_targets, hypotheses);


    /* 将起始成功的目标移动到已跟踪目标列表 */
    this->move_confirmed_targets(tracked_targets, unconfirmed_targets);


    /*  从仍没有被关联的测量值中选取可能的值生成新跟踪 */
    this->creat_targets(unconfirmed_targets, measurements, timestamp_ms);

    return;
}


void Initiator::update_lifecycle(std::vector<Hypothesis> &hypotheses,LifeCycle &life_cycle)
{
    int32_t score = 0;
    uint32_t speed = 0;
    for(int i = 0; i < hypotheses.size(); i++) {
        rd_float_t dt = (hypotheses[0].prediction.timestamp_ms - hypotheses[0].prior_state.timestamp_ms) / 1000; //计算时间戳差的秒数
    Vector3r empty_measurement = Vector3r::Zero();
    if (hypotheses[0].measurement == empty_measurement) {
        //关联失败，超时了，扣除一半分数
        if (life_cycle.unassociated_time > this->unassociated_time) {
            score -= life_cycle.score >> 1;// 用右移代替除以2
            score += int32_t(this->unassociated_time * dt); //加上未关联时间乘时间差
            life_cycle.unassociated_time += dt;
        }
    }else {
        score -= life_cycle.unassociated_time * this->unassociated_score /2;
        speed = std::abs(hypotheses[0].measurement[3]);
        if (speed > this->speed_threshold) {
            score += int32_t(this->motion_score * dt);
        }else {
            score += this->static_score * dt;
        }
        life_cycle.unassociated_time = 0;
    }
    life_cycle.score += int32_t(score);     
    }
    return; 
}


void Initiator::move_confirmed_targets(TrackedTargets &tracked_targets, TrackedTargets &unconfirmed_targets)
{
    return;
}


void Initiator::creat_targets(TrackedTargets &unconfirmed_targets, std::vector<Vector3r> &measurements, uint32_t timestamp_ms)
{
    return;
}

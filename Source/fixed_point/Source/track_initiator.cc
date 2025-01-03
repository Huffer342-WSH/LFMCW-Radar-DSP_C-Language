#include "track_initiator.hh"

#include "track_kalman.hh"


void Initiator::initiate(TrackedTargets &tracked_targets, TrackedTargets &unconfirmed_targets, std::vector<Vector3r> &measurements, uint32_t timestamp_ms)
{

    std::vector<Hypothesis> hypotheses; // 在子代码块中的局部变量会在代码块结束后被销毁
    this->associator.hypotheses_init(hypotheses, unconfirmed_targets, timestamp_ms);

    /* 数据关联 */
    RD_DEBUG("[目标跟踪] 数据关联");
    this->associator.associate(hypotheses, measurements, timestamp_ms);


    /* 滤波：更新器根据假设计算后验状态，并赋值给目标 */
    RD_DEBUG("[目标跟踪] 滤波");
    this->associator.update(unconfirmed_targets, hypotheses);


    /*  更新生命周期 */
    this->update_lifecycle(unconfirmed_targets, hypotheses);


    /* 将起始成功的目标移动到已跟踪目标列表 */
    this->move_confirmed_targets(tracked_targets, unconfirmed_targets);


    /*  从仍没有被关联的测量值中选取可能的值生成新跟踪 */
    this->creat_targets(unconfirmed_targets, measurements, timestamp_ms);

    return;
}


void Initiator::update_lifecycle(TrackedTargets &targets, std::vector<Hypothesis> hypotheses)
{

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

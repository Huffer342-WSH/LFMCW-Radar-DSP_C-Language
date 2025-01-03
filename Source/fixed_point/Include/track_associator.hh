#pragma once

#include "track_kalman.hh"
#include "track_target.hh"


class Associator
{
private:
public:
    rd_float_t missed_distance;
    KalmanPredictor &predictor;
    KalmanUpdater &updater;

    Associator(KalmanPredictor &predictor, KalmanUpdater &updater, rd_float_t missed_distance)
        : predictor(predictor)
        , updater(updater)
        , missed_distance(missed_distance) {};
    ~Associator() { };


    rd_float_t distance(Hypothesis &hypothesis, Vector3r &measurement);


    void hypotheses_init(std::vector<Hypothesis> &hypotheses, TrackedTargets &targets, uint32_t timestamp_ms);


    /**
     * @brief 数据关联
     *
     * @details 将假设中的先验状态和测量值进行关联，匹配结果保存到对应假设中；未使用的测量值保存在measurements头部
     *
     * @param[in,out]   hypotheses          假设；输入时假设中需要包含先验状态， 关联后将测量值和预测值保存在假设中
     * @param[in,out]   measurements        测量值；未关联的测量值会移动到measurements头部
     * @param           timestamp_ms        时间戳
     */
    void associate(std::vector<Hypothesis> &hypotheses, std::vector<Vector3r> &measurements, uint32_t timestamp_ms);


    /**
     * @brief 状态更新
     *
     * @details 调用卡尔曼更新器，处理每一个目标
     *
     * @param targets
     * @param hypotheses
     */
    void update(TrackedTargets &targets, std::vector<Hypothesis> &hypotheses);
};

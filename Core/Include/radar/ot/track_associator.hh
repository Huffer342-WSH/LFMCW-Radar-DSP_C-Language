#pragma once

#include <radar/ot/track_kalman.hh>
#include <radar/ot/track_target.hh>
#include <vector>

class Associator
{
private:
    KalmanPredictor &predictor;
    KalmanUpdater &updater;

public:
    rd_float_t missed_distance = 5; ///< 无法关联的最大距离
    rd_float_t wr = 1;              ///< 距离权重
    rd_float_t wv = 1;              ///< 速度权重

    Associator(KalmanPredictor &predictor, KalmanUpdater &updater, rd_float_t missed_distance)
        : predictor(predictor)
        , updater(updater)
        , missed_distance(missed_distance) { };


    Associator(KalmanPredictor &predictor, KalmanUpdater &updater, rd_float_t missed_distance, rd_float_t wr, rd_float_t wv)
        : predictor(predictor)
        , updater(updater)
        , missed_distance(missed_distance)
        , wr(wr)
        , wv(wv) { };
    ~Associator() { };


    rd_float_t distance(Hypothesis &hypothesis, Vector3r &measurement, rd_float_t wr, rd_float_t wv);


    void hypotheses_init(std::vector<Hypothesis> &hypotheses, TrackedTargets &targets);

    std::vector<Hypothesis> hypotheses_new(TrackedTargets &targets);


    /**
     * @brief 数据关联
     *
     * @details 将假设中的先验状态和测量值进行关联，匹配结果保存到对应假设中；未使用的测量值保存在measurements头部
     *
     * @param[in,out]   hypotheses          假设；输入时假设中需要包含先验状态， 关联后将测量值和预测值保存在假设中
     * @param[in,out]   measurements        测量值；未关联的测量值会移动到measurements头部
     * @param           timestamp_ms        时间戳
     * @param           missed_distance     关联成功的最大距离
     */
    void associate(std::vector<Hypothesis> &hypotheses, std::vector<Vector3r> &measurements, uint32_t timestamp_ms, rd_float_t missed_distance);


    void associate(std::vector<Hypothesis> &hypotheses, std::vector<Vector3r> &measurements, uint32_t timestamp_ms)
    {
        associate(hypotheses, measurements, timestamp_ms, this->missed_distance);
    }


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

#include "track_associator.hh"
#include "radar_log.h"
#include <cmath>


void Associator::hypotheses_init(std::vector<Hypothesis> &hypotheses, TrackedTargets &targets, uint32_t timestamp_ms)
{
    hypotheses.clear();
    hypotheses.reserve(targets.size());
    for (tracked_target_t &target : targets) {
        gaussian_state_t *state = target.state;
        hypotheses.emplace_back(state->state_vector, state->covar, timestamp_ms);
    }
    return;
}


/**
 * @brief 数据关联
 *
 * @details 将假设中的先验状态和测量值进行关联，匹配结果保存到对应假设中；未使用的测量值保存在measurements头部
 *
 * @param[in,out]   hypotheses      假设；输入时假设中需要包含先验状态， 关联后将测量值和预测值保存在假设中
 * @param[in,out]   measurements    测量值；未关联的测量值会移动到measurements头部
 * @param           timestamp_ms    时间戳
 */
void Associator::associate(std::vector<Hypothesis> &hypotheses, std::vector<Eigen::Vector3d> &measurements, uint32_t timestamp_ms)
{

    /* 预测状态向量： 将预测状态写入到假设中 */
    RD_DEBUG("[数据关联] 预测状态向量");
    for (Hypothesis &hypothesis : hypotheses) {
        predictor.predict(hypothesis.prediction, hypothesis.prior_state, timestamp_ms);
    }


    /* 预测测量值： 使用测量模型将预测状态转化成预测测量值，写入假设中 */
    RD_DEBUG("[数据关联] 预测测量值：");
    for (Hypothesis &hypothesis : hypotheses) {
        updater.predict_measurement(hypothesis.measurement_prediction, hypothesis.prediction);
    }


    /* 生成距离矩阵 */
    RD_DEBUG("[数据关联] 生成距离矩阵");
    size_t M = hypotheses.size(), N = measurements.size();
    std::vector<double> distance_matrix(M * (M + N), INFINITY);
    for (size_t i = 0; i < M; i++) {
        double *row = &distance_matrix[i * (N + M)];
        for (size_t j = 0; j < measurements.size(); j++) {
            row[j] = distance(hypotheses[i], measurements[j]);
        }
    }
    for (size_t i = 0; i < M; i++) {
        distance_matrix[i * (M + N) + M + i] = missed_distance;
    }

    /* GNN数据关联 */

    /* 将关联结果写入假设，整理量测值 */
    return;
};


/**
 * @brief  计算假设中的预测测量值和测量值之间的距离
 *
 * @param hypothesis 假设
 * @param measurement 测量值
 * @return double
 */
double Associator::distance(Hypothesis &hypothesis, Eigen::Vector3d &measurement)
{
    return 0;
}


void Associator::update(TrackedTargets &targets, std::vector<Hypothesis> &hypotheses)
{

    RADAR_ASSERT_EQ((size_t)targets.size(), (size_t)hypotheses.size());

    return;
}

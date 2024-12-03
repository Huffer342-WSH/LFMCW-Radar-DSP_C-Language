
#include "track_kalman.hh"


/**
 * @brief 卡尔曼预测
 *
 * @param[out] predict_state 预测状态
 * @param[in]  state         先验状态
 * @param[in]  timestamp_ms  时间戳
 */
void KalmanPredictor::predict(GaussianState &predict_state, GaussianState &state, uint32_t timestamp_ms)
{
    return;
}


/**
 * @brief 卡尔曼滤波
 *
 * @param[out] post         滤波后
 * @param[ in] hypothesis   假设，内部包含预测值和测量值
 */
void KalmanUpdater::update(GaussianState &post, Hypothesis &hypothesis)
{
}


/**
 * @brief 卡尔曼测量预测
 *
 * @param measurement_prediction  测量预测
 * @param predicted_state         预测状态
 */
void KalmanUpdater::predict_measurement(GaussianMeasurementPrediction &measurement_prediction, GaussianState &predicted_state)
{

    return;
}

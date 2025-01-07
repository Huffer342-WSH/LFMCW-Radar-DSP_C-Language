
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
    rd_float_t dt = (rd_float_t)(timestamp_ms - state.timestamp_ms) / 1000;

    // 时间戳
    predict_state.timestamp_ms = timestamp_ms;

    // x 状态向量
    transition_model.function(predict_state.state_vector, state.state_vector, dt);

    // P 误差协方差矩阵
    Matrix44r F, Q, &P = state.covar;
    transition_model.matrix(F, dt);
    transition_model.covar(Q, dt);
    predict_state.covar = F * P * F.transpose() + Q;

    return;
}


/**
 * @brief 卡尔曼测量预测
 *
 * @param measurement_prediction  测量预测
 * @param predicted_state         预测状态
 */
void KalmanUpdater::predict_measurement(GaussianMeasurementPrediction &measurement_prediction, const GaussianState &predicted_state)
{
    // 提取状态向量和协方差矩阵
    const Vector4r &x = predicted_state.state_vector;
    const Matrix44r &P = predicted_state.covar;

    // 预测测量值
    measurement_model.function(measurement_prediction.state_vector, x);

    // 获取测量模型的雅可比矩阵和协方差矩阵
    measurement_model.matrix(measurement_prediction.meas_matrix, x);

    measurement_model.covar(measurement_prediction.R);

    // 计算交叉协方差
    Matrix34r &H = measurement_prediction.meas_matrix;
    measurement_prediction.cross_covar = P * H.transpose();

    // 计算新息协方差
    Matrix33r &R = measurement_prediction.R;
    measurement_prediction.covar = H * measurement_prediction.cross_covar + R;

    // 时间戳保持一致
    measurement_prediction.timestamp_ms = predicted_state.timestamp_ms;
}


/**
 * @brief 卡尔曼滤波
 *
 * @param[out] post         滤波后
 * @param[ in] hypothesis   假设，内部包含预测值和测量值
 */
void KalmanUpdater::update(GaussianState &post, Hypothesis &hypothesis, bool need_pred_meas)
{
    // 预测测量值
    if (need_pred_meas) { }
    predict_measurement(hypothesis.measurement_prediction, hypothesis.prediction);


    // 提取必要的矩阵和向量
    const Matrix33r &S = hypothesis.measurement_prediction.covar;
    const Matrix43r &PH_T = hypothesis.measurement_prediction.cross_covar;
    const Matrix34r &H = hypothesis.measurement_prediction.meas_matrix;
    const Matrix44r &P_pred = hypothesis.prediction.covar;
    const Matrix33r &R = hypothesis.measurement_prediction.R;
    const Vector3r &z = hypothesis.measurement;
    const Vector3r &z_pred = hypothesis.measurement_prediction.state_vector;
    const Vector4r &x_pred = hypothesis.prediction.state_vector;

    // 计算卡尔曼增益 K = P*H^T * S^(-1)
    Matrix43r K = S.transpose().fullPivLu().solve(PH_T.transpose()).transpose();

    // 更新后验协方差矩阵 P = (I - K*H)*P_pred*(I - K*H)^T + K*R*K^T
    Matrix44r I_KH = Matrix44r::Identity() - K * H;
    post.covar = I_KH * P_pred * I_KH.transpose() + K * R * K.transpose();

    // 更新后验状态向量 x = x_pred + K * (z - z_pred)
    post.state_vector = x_pred + K * (z - z_pred);

    post.timestamp_ms = hypothesis.prediction.timestamp_ms;
}

#pragma once

#include <Eigen/Dense>
#include "track_model.hh"

class GaussianState
{
private:
public:
    Eigen::Vector4d state_vector; // x 状态向量
    Eigen::Matrix4d covar;        // P 协方差矩阵

    uint32_t timestamp_ms;        // 时间戳

    // 构造函数
    GaussianState() { };

    GaussianState(const Eigen::Vector4d &state_vector, const Eigen::Matrix4d &covar, uint32_t timestamp_ms)
        : state_vector(state_vector)
        , covar(covar)
        , timestamp_ms(timestamp_ms) { };


    GaussianState(double *state_vector, double *covar, uint32_t timestamp_ms)
        : state_vector(Eigen::Map<Eigen::Vector4d>(state_vector))
        , covar(Eigen::Map<Eigen::Matrix4d>(covar))
        , timestamp_ms(timestamp_ms) { };
};


class GaussianMeasurementPrediction
{
private:
public:
    Eigen::Vector3d state_vector;            // z_pred   = h(x_pred)
    Eigen::Matrix3d covar;                   // S        = H * P * H^T
    Eigen::Matrix<double, 3, 4> cross_covar; // P * H^T

    uint32_t timestamp_ms;                   // 时间戳

    GaussianMeasurementPrediction() { };

    GaussianMeasurementPrediction(const Eigen::Vector3d &state_vector, const Eigen::Matrix3d &covar, const Eigen::Matrix<double, 3, 4> &cross_covar,
                                  uint32_t timestamp_ms)
        : state_vector(state_vector)
        , covar(covar)
        , cross_covar(cross_covar)
        , timestamp_ms(timestamp_ms) { };
};


class Hypothesis
{
private:
public:
    Eigen::Vector3d measurement;
    GaussianState prior_state;
    GaussianState prediction;
    GaussianMeasurementPrediction measurement_prediction;

    Hypothesis(double prior_state_vector[4], double prior_state_covar[16], uint32_t timestamp_ms)
        : prior_state(prior_state_vector, prior_state_covar, timestamp_ms) { };
};


class KalmanPredictor
{
public:
    TransitionModel transition_model;
    KalmanPredictor(double q)
        : transition_model(q) { };
    ~KalmanPredictor() { };


    void predict(GaussianState &predict_state, GaussianState &state, uint32_t timestamp_ms);
};


class KalmanUpdater
{
public:
    MeasurementModel measurement_model;
    KalmanUpdater(double sigma_phi, double sigma_r, double sigma_r_dot)
        : measurement_model(sigma_phi, sigma_r, sigma_r_dot)
    {
    }
    ~KalmanUpdater() { };


    void update(GaussianState &post, Hypothesis &hypothesis);
    void predict_measurement(GaussianMeasurementPrediction &measurement_prediction, GaussianState &predicted_state);
};

#pragma once

#include "radar_assert.h"
#include "radar_math.h"
#include "track_model.hh"

class GaussianState
{
private:
public:
    Vector4r state_vector; // x 状态向量
    Matrix44r covar;       // P 协方差矩阵

    uint32_t timestamp_ms;        // 时间戳

    // 构造函数
    GaussianState() { };

    GaussianState(const Vector4r &state_vector, const Matrix44r &covar, uint32_t timestamp_ms)
        : state_vector(state_vector)
        , covar(covar)
        , timestamp_ms(timestamp_ms) {};


    GaussianState(rd_float_t *state_vector, rd_float_t *covar, uint32_t timestamp_ms)
        : state_vector(Eigen::Map<Vector4r>(state_vector))
        , covar(Eigen::Map<Matrix44r>(covar))
        , timestamp_ms(timestamp_ms) {};
};


class GaussianMeasurementPrediction
{
private:
public:
    Vector3r state_vector; // z_pred   = h(x_pred)
    Matrix33r R;
    Matrix33r covar;       // S        = H * P * H^T
    Matrix43r cross_covar; // P * H^T
    Matrix34r meas_matrix; // H

    uint32_t timestamp_ms;                   // 时间戳

    GaussianMeasurementPrediction() { };

    GaussianMeasurementPrediction(const Vector3r &state_vector, const Matrix33r &covar, const Matrix43r &cross_covar, uint32_t timestamp_ms)
        : state_vector(state_vector)
        , covar(covar)
        , cross_covar(cross_covar)
        , timestamp_ms(timestamp_ms) {};
};


class Hypothesis
{
private:
public:
    Vector3r measurement;
    GaussianState prior_state;
    GaussianState prediction;
    GaussianMeasurementPrediction measurement_prediction;

    Hypothesis(rd_float_t prior_state_vector[4], rd_float_t prior_state_covar[16], uint32_t timestamp_ms)
        : prior_state(prior_state_vector, prior_state_covar, timestamp_ms) {};

    Hypothesis(GaussianState &prior_state)
        : prior_state(prior_state) {};

    ~Hypothesis() {};
};


class KalmanPredictor
{
public:
    TransitionModel transition_model;
    KalmanPredictor(rd_float_t q)
        : transition_model(q) {};
    ~KalmanPredictor() { };


    void predict(GaussianState &predict_state, GaussianState &state, uint32_t timestamp_ms);
};


class KalmanUpdater
{
public:
    MeasurementModel measurement_model;
    KalmanUpdater(rd_float_t sigma_phi, rd_float_t sigma_r, rd_float_t sigma_r_dot)
        : measurement_model(sigma_phi, sigma_r, sigma_r_dot)
    {
    }
    KalmanUpdater(Matrix33r &R)
        : measurement_model(R) {};

    ~KalmanUpdater() { };


    void update(GaussianState &post, Hypothesis &hypothesis, bool need_pred_meas);
    void predict_measurement(GaussianMeasurementPrediction &measurement_prediction, const GaussianState &predicted_state);
};

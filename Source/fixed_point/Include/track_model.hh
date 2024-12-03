#pragma once


#include <Eigen/Dense>
#include "radar_math.h"

class TransitionModel
{
private:
    double q; // 过程噪声系数
public:
    TransitionModel(double q);
    ~TransitionModel() { };

    size_t ndim_state;
    void function(Eigen::Vector4d &predict_state_vector, Eigen::Vector4d &state_vector, double *deltatime);
    void matrix(Eigen::Matrix4d &F, double *deltatime);
    void covar(Eigen::Matrix4d *Q, float deltatime);
};

class MeasurementModel
{
private:
    Eigen::Matrix3d R;

public:
    MeasurementModel(double sigma_phi, double sigma_r, double sigma_r_dot);
    ~MeasurementModel() { };

    size_t ndim_state;
    size_t ndim_meas;

    void function(Eigen::Vector3d &meas_vector, Eigen::Vector4d &stata_vector);
    void matrix(Eigen::Matrix<double, 3, 4> &H, Eigen::Vector4d &stata_vector);
    void covar(Eigen::Matrix3d &R);
};

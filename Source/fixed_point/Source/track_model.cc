#include "track_model.hh"


TransitionModel::TransitionModel(double q)
    : q(q)
{
}


void TransitionModel::function(Eigen::Vector4d &predict_state_vector, Eigen::Vector4d &state_vector, double *deltatime)
{
}

void TransitionModel::matrix(Eigen::Matrix4d &F, double *deltatime)
{
}


void TransitionModel::covar(Eigen::Matrix4d *Q, float deltatime)
{
}


MeasurementModel::MeasurementModel(double sigma_phi, double sigma_r, double sigma_r_dot)
{
    R.setZero();
    R(0, 0) = sigma_phi * sigma_phi;
    R(1, 1) = sigma_r * sigma_r;
    R(2, 2) = sigma_r_dot * sigma_r_dot;
}


void MeasurementModel::function(Eigen::Vector3d &meas_vector, Eigen::Vector4d &stata_vector)
{
}

void MeasurementModel::matrix(Eigen::Matrix<double, 3, 4> &H, Eigen::Vector4d &stata_vector)
{
}


void MeasurementModel::covar(Eigen::Matrix3d &R)
{
}

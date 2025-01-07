#pragma once

#include "radar_assert.h"
#include "radar_math.h"


class TransitionModel
{
private:
    rd_float_t q; // 过程噪声系数
public:
    TransitionModel(rd_float_t q)
        : q(q) {};
    ~TransitionModel() { };

    size_t ndim_state;
    void function(Vector4r &predict_state_vector, Vector4r &state_vector, rd_float_t deltatime);
    void matrix(Matrix44r &F, rd_float_t deltatime);
    void covar(Matrix44r &Q, rd_float_t deltatime);
};

class MeasurementModel
{
private:
    Matrix33r R;

public:
    MeasurementModel(rd_float_t sigma_phi, rd_float_t sigma_r, rd_float_t sigma_r_dot);
    MeasurementModel(Matrix33r &R)
        : R(R) {};
    ~MeasurementModel() { };

    size_t ndim_state;
    size_t ndim_meas;

    void function(Vector3r &meas_vector, const Vector4r &state_vector);
    void matrix(Matrix34r &H, const Vector4r &state_vector);
    void covar(Matrix33r &R);
};

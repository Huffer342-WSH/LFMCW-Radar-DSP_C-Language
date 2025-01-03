#include "track_model.hh"


/**
 * @brief 输入状态向量，输出预测状态向量
 *
 * @param predict_state_vector
 * @param state_vector
 * @param deltatime
 */
void TransitionModel::function(Vector4r &predict_state_vector, Vector4r &state_vector, rd_float_t deltatime)
{
    predict_state_vector(0) = state_vector(0) + state_vector(1) * deltatime;
    predict_state_vector(1) = state_vector(1);
    predict_state_vector(2) = state_vector(2) + state_vector(3) * deltatime;
    predict_state_vector(3) = state_vector(3);
    return;
}


void TransitionModel::matrix(Matrix44r &F, rd_float_t deltatime)
{
    F.setIdentity();
    F(0, 1) = deltatime;
    F(2, 3) = deltatime;
    return;
}


void TransitionModel::covar(Matrix44r &Q, rd_float_t deltatime)
{
    rd_float_t d, d2, d3;
    d = deltatime * this->q;
    d2 = d * deltatime;
    d3 = d2 * deltatime;
    d2 /= 2;
    d3 /= 3;
    Q.setZero();
    Q(0, 0) = Q(2, 2) = d3;
    Q(0, 1) = Q(2, 3) = Q(1, 0) = Q(3, 2) = d2;
    Q(1, 1) = Q(3, 3) = d;
    return;
}


MeasurementModel::MeasurementModel(rd_float_t sigma_phi, rd_float_t sigma_r, rd_float_t sigma_r_dot)
{
    this->R.setZero();
    this->R(0, 0) = sigma_phi * sigma_phi;
    this->R(1, 1) = sigma_r * sigma_r;
    this->R(2, 2) = sigma_r_dot * sigma_r_dot;
}


void MeasurementModel::function(Vector3r &meas_vector, const Vector4r &state_vector)
{
    rd_float_t x = state_vector(0);
    rd_float_t y = state_vector(2);
    rd_float_t rho = hypot(x, y);
    rd_float_t rho_rate = (x * state_vector(1) + y * state_vector(3)) / rho;
    meas_vector(0) = atan2(y, x);
    meas_vector(1) = rho;
    meas_vector(2) = rho_rate;
}

/**
 * @brief
 *
 * @param H
 * @param state_vector
 *
 * @details
 *  测量向量 [phi, r, rdot]'
 *  状态向量 [x, vx, y, vy]'
 *
 *  phi  = atan(y/x)
 *  r    = sqrt(x^2 + y^2)
 *  rdot = (x*vx + y*vy)/sqrt(x^2 + y^2)
 *
 */
void MeasurementModel::matrix(Matrix34r &H, const Vector4r &state_vector)
{
    rd_float_t x = state_vector(0), vx = state_vector(1), y = state_vector(2), vy = state_vector(3);

    rd_float_t r2 = x * x + y * y; // r^2 = x^2 + y^2
    rd_float_t r = hypot(x, y);    // r   = sqrt(x^2 + y^2)
    rd_float_t r3 = r2 * r;        // r^3

    H.setZero();

    // 距离为0的特殊情况
    if (r3 == 0) {
        rd_float_t vr = hypot(vx, vy);
        if (vr > 0.0001) {
            H(2, 1) = vx / vr;
            H(2, 3) = vy / vr;
        }
        return;
    }


    H(0, 0) = -y / r2; // dphi/dx
    H(0, 2) = x / r2;  // dphi/dy

    H(1, 0) = H(2, 1) = x / r; // dr/dx and drdot/dvx
    H(1, 2) = H(2, 3) = y / r; // dr/dy and drdot/dvy


    H(2, 0) = (y * (x * vy - y * vx)) / r3; // drdot/dx
    H(2, 2) = (x * (y * vx - x * vy)) / r3; // drdot/dy
}

void MeasurementModel::covar(Matrix33r &R)
{
    R = this->R;
}

#include "track_kalman.hh"


#include <iostream>
#include <gtest/gtest.h>

using namespace std;

#include "./test_track_kalman.data"

TEST(RadarFixedTest, KalmanFilter)
{
    Matrix33r R;
    Vector4r x;
    Matrix44r P;
    uint32_t timestamp_ms = 0;

    R = Eigen::Map<Eigen::Matrix<double, 3, 3, Eigen::RowMajor>>((double *)_R0).cast<rd_float_t>();
    x = Eigen::Map<Eigen::Matrix<double, 4, 1>>((double *)_x0).cast<rd_float_t>();
    P = Eigen::Map<Eigen::Matrix<double, 4, 4, Eigen::RowMajor>>((double *)_P0).cast<rd_float_t>();


    KalmanPredictor predictor = KalmanPredictor(_q);
    KalmanUpdater updater = KalmanUpdater(R);
    GaussianState prior_state(x, P, timestamp_ms);
    Hypothesis hypothesis(prior_state);

    double max_error = 0;
    Eigen::Matrix<double, 4, 1> mean_square_error = Eigen::Matrix<double, 4, 1>::Zero();

    for (size_t i = 0; i < N; i++) {
        predictor.predict(hypothesis.prediction, hypothesis.prior_state, timestamps[i]);
        updater.predict_measurement(hypothesis.measurement_prediction, hypothesis.prediction);
        hypothesis.measurement = Eigen::Map<Eigen::Matrix<double, 3, 1>>((double *)_z_list[i]).cast<rd_float_t>();
        updater.update(hypothesis.prior_state, hypothesis, true);

        cout << "第 " << i << " 帧  " << "时间戳:" << timestamps[i] << endl;
        cout << "预测:" << endl;
        cout << "x >>\n" << hypothesis.prediction.state_vector << '\n' << endl;
        cout << "P >>\n" << hypothesis.prediction.covar << '\n' << endl;
        cout << "z >>\n" << hypothesis.measurement_prediction.state_vector << '\n' << endl;
        cout << "更新:" << endl;
        cout << "z >>\n" << hypothesis.measurement << '\n' << endl;
        cout << "S >>\n" << hypothesis.measurement_prediction.covar << '\n' << endl;
        cout << "PH' >>\n" << hypothesis.measurement_prediction.cross_covar << '\n' << endl;
        cout << "x >>\n" << hypothesis.prior_state.state_vector << '\n' << endl;
        cout << "P >>\n" << hypothesis.prior_state.covar << '\n' << endl;

        Eigen::Matrix<rd_float_t, 4, 1> x_correct = Eigen::Map<Eigen::Matrix<double, 4, 1>>((double *)_x_list[i]).cast<rd_float_t>();
        Eigen::Matrix<rd_float_t, 4, 1> error = (x_correct - hypothesis.prior_state.state_vector).cwiseAbs();
        for (int i = 0; i < error.size(); ++i) {
            max_error = max(error(i), max_error);
        }
        mean_square_error += error.cwiseAbs2();
    }
    mean_square_error /= N;
    cout << "均方误差:\n" << mean_square_error << endl;
    cout << "最大误差:\n" << max_error << endl;
    ASSERT_NEAR(max_error, 0.0, 1e-11);
    for (int i = 0; i < mean_square_error.size(); ++i) {
        ASSERT_NEAR(sqrt(mean_square_error(i)), 0.0, 1e-11);
    }
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

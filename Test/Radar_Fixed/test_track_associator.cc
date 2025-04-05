#include <radar/ot/track_associator.hh>


#include <iostream>
#include <gtest/gtest.h>
#include <cmath>

const int M = 5;
const int N = 0;


rd_float_t target_pos[][4] = {
    { 0,  0, 0,  0 },
    { 1,  0, 1,  0 },
    { -1, 0, 1,  0 },
    { -1, 0, -1, 0 },
    { 1,  0, -1, 0 }
};
rd_float_t meas[][4] = {
    { 0, 0, 0, 0 },
    { 1, 0, 1, 0 },
    { 2, 0, 2, 0 }
};


TEST(RadarFixedTest, associate)
{
    GaussianState state;
    TrackedTargets tracked_targets;
    for (int i = 0; i < M; i++) {
        GaussianState state(Vector4r(target_pos[i]), Matrix44r::Identity(), 0);
        tracked_targets.emplace_back(i, state);
    }
    std::vector<Vector3r> measurements;
    for (int i = 0; i < N; i++) {
        rd_float_t *a = meas[i];
        rd_float_t theta = atan2(a[2], a[0]);
        rd_float_t r = hypot(a[0], a[2]);
        rd_float_t v;
        if (a[1] == 0 && a[3] == 0) {
            v = 0;
        } else {
            v = hypot(a[1], a[3]);
        }
        measurements.push_back(Vector3r(theta, r, v));
    }

    KalmanPredictor predictor(0.1);
    KalmanUpdater updater(0.1, 0.1, 0.1);
    Associator associator(predictor, updater, 0.1);

    std::vector<Hypothesis> hypotheses = associator.hypotheses_new(tracked_targets);
    associator.associate(hypotheses, measurements, 1);

    /* 打印结果 */
    printf("假设结果：\n");
    for (int i = 0; i < hypotheses.size(); i++) {
        Hypothesis &h = hypotheses[i];
        printf("假设%d:\n", i);
        printf("目标坐标:      \t[%f %f %f %f]\n", h.prior_state.state_vector[0], h.prior_state.state_vector[1], h.prior_state.state_vector[2],
               h.prior_state.state_vector[3]);
        printf("预测后目标测量值: \t[%f %f %f]\n", h.measurement_prediction.state_vector[0], h.measurement_prediction.state_vector[1],
               h.measurement_prediction.state_vector[2]);
        if (h.has_meas) {
            printf("测量值       \t[%f %f %f]\n", h.measurement[0], h.measurement[1], h.measurement[2]);
        } else {
            printf("测量值       \t无\n");
        }
    }


    printf("剩余测量值：\n");
    for (Vector3r &m : measurements) {
        printf("[%f %f %f]\n", m[0], m[1], m[2]);
    }
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

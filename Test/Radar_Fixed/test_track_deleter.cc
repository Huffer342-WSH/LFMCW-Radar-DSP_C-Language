#include <radar/ot/track_deleter.hh>
#include <gtest/gtest.h>
static const rd_float_t pi = 3.14159; // pi
rd_float_t fov[2] = { -pi / 3, pi / 3 };
rd_float_t radius_range[2] = { 0.2, 100 };
TEST(RadarFixedtest1, unassoiatedTrack_inspace)
{
    static const rd_float_t pi = 3.14159; // pi
    std::vector<Hypothesis> hypotheses;
    TrackedTargets tracked_targets;
    Deleter deleter(10.0, 0.2, fov, radius_range); // unassociated_time, missed_probability, fov, radius_range
    rd_float_t prior_state_vector[4] = { 2.0, 2.0, 3.0, 4.0 };     // 测量范围内，
    rd_float_t prior_state_covar[16] = { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };
    uint32_t timestamp_ms = 0;
    GaussianState prior_state(prior_state_vector, prior_state_covar, timestamp_ms);
    Hypothesis hypothesis(prior_state);
    hypothesis.prediction.timestamp_ms = 2000;
    hypothesis.prior_state.timestamp_ms = 1000;
    hypothesis.has_meas = false; // 测量值为空，无法关联
    hypotheses.push_back(hypothesis);

    TrackedTarget tracked_target(1, prior_state);
    tracked_target.life_cycle.score = 10;
    tracked_targets.push_back(tracked_target);

    deleter.update_lifecycle(tracked_targets, hypotheses);
    EXPECT_EQ(tracked_targets.front().life_cycle.score, -490);
    EXPECT_EQ(tracked_targets.front().life_cycle.unassociated_time, 1); // 如果没有关联，则unassociated_time 加上 时间差，原始2加dt 1 结果为3，单位为s
}

TEST(RadarFixedtest2, assoiatedTrack_inspace)
{
    static const rd_float_t pi = 3.14159; // pi
    std::vector<Hypothesis> hypotheses;
    TrackedTargets tracked_targets;
    Deleter deleter(10.0, 0.2, fov, radius_range); // unassociated_time, missed_probability, fov, radius_range
    rd_float_t prior_state_vector[4] = { 2.0, 2.0, 3.0, 4.0 };     // 测量范围内，
    rd_float_t prior_state_covar[16] = { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };
    uint32_t timestamp_ms = 0;
    GaussianState prior_state(prior_state_vector, prior_state_covar, timestamp_ms);
    Hypothesis hypothesis(prior_state);
    hypothesis.prediction.timestamp_ms = 2000;
    hypothesis.prior_state.timestamp_ms = 1000;
    hypothesis.has_meas = true; // 测量值不为空，关联
    hypotheses.push_back(hypothesis);

    TrackedTarget tracked_target(1, prior_state);
    tracked_target.life_cycle.score = 10;
    tracked_targets.push_back(tracked_target);

    deleter.update_lifecycle(tracked_targets, hypotheses);
    EXPECT_EQ(tracked_targets.front().life_cycle.score, 135);
    EXPECT_EQ(tracked_targets.front().life_cycle.unassociated_time, 0); // 如果关联，则unassociated_time 重置为0
}


TEST(RadarFixedtest3, outspace)
{
    static const rd_float_t pi = 3.14159; // pi
    std::vector<Hypothesis> hypotheses;
    TrackedTargets tracked_targets;
    Deleter deleter(10.0, 0.2, fov, radius_range); // unassociated_time, missed_probability, fov, radius_range
    rd_float_t prior_state_vector[4] = { 0.5, 2.0, 3.0, 4.0 };     // 测量范围内，
    rd_float_t prior_state_covar[16] = { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };
    uint32_t timestamp_ms = 0;
    GaussianState prior_state(prior_state_vector, prior_state_covar, timestamp_ms);
    Hypothesis hypothesis(prior_state);
    hypothesis.prediction.timestamp_ms = 2000;
    hypothesis.prior_state.timestamp_ms = 1000;
    hypotheses.push_back(hypothesis);

    TrackedTarget tracked_target(1, prior_state);
    tracked_target.life_cycle.score = 10;
    tracked_targets.push_back(tracked_target);

    deleter.update_lifecycle(tracked_targets, hypotheses);

    EXPECT_EQ(tracked_targets.front().life_cycle.score, -1); // 如果没有关联，则unassociated_time 加上 时间差，原始2加dt 1 结果为3，单位为s
}
TEST(RadarFixedtest3, track_deleter)
{
    static const int N = 8;
    static int32_t targets_score[] = { 10, -1, 0, 100, 0, -1, 100, 0 };

    TrackedTargets targets;
    for (int i = 0; i < N; i++) {
        GaussianState state;
        TrackedTarget t(i, state);
        t.life_cycle.score = targets_score[i];
        targets.push_back(t);
    }

    printf("Targets Info:\n");
    for (auto &t : targets) {
        printf("UUID:%d Score %d\n", t.uuid, t.life_cycle.score);
    }

    // 创建 Deleter 类的测试数据
    Deleter deleter(10.0, 0.2, fov, radius_range); // unassociated_time, missed_probability, fov, radius_range

    // deleter.delete_invalid_targets();

    printf("After delete:\n");
    for (auto &t : targets) {
        printf("UUID:%d Score %d\n", t.uuid, t.life_cycle.score);
    }
}
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

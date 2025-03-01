
#include "track_initiator.hh"
#include <cmath>
#include <gtest/gtest.h>
#include <iostream>

const int M = 5;
const int N = 0;

TEST(RadarFixedTest1, update_lifecycle_unassociated_timeout)
{
    KalmanPredictor predictor(0.1);
    KalmanUpdater updater(0.1, 0.1, 0.1);
    Associator associator(predictor, updater, 0.1);
    Initiator initiator(associator, 1.0, 5.0, 5.0, 2.0, 1.0); // 初始化超时1s

    std::vector<Hypothesis> hypotheses;

    TrackedTargets tracked_targets;

    rd_float_t prior_state_vector[4] = { 1.0, 2.0, 3.0, 4.0 };
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
    tracked_target.life_cycle.unassociated_time = 2; // 2s未关联，大于初始化超时时间
    tracked_targets.push_back(tracked_target);


    // 列表里只有一个类
    printf("unassociated_time: %f\n", tracked_target.life_cycle.unassociated_time);
    printf("score: %d\n", tracked_target.life_cycle.score);
    std::cout << "\n----------------------------\n";
    initiator.update_lifecycle(tracked_targets, hypotheses);

    std::cout << "\n----------------------------\n";

    printf("unassociated_score: %d\n", initiator.unassociated_score); // unassociated_score = - initial score / unassociated time = -1000 / 1 = -1000
    printf("score: %d\n", tracked_targets.front().life_cycle.score);

    EXPECT_EQ(tracked_targets.front().life_cycle.unassociated_time, 3); // 如果没有关联，则unassociated_time 加上 时间差，原始2加dt 1 结果为3，单位为s
    EXPECT_EQ(
        tracked_targets.front().life_cycle.score,
        -995); // 如果没有关联且超时，变化score原值是0，减去生命周期分数的一半，有加上未关联分数(-1000)*dt(1)，为-1005，原本生命周期分数为10，加上变化score，结果为-995
}

TEST(RadarFixedTest2, update_lifecycle_associated_untimeout)
{
    KalmanPredictor predictor(0.1);
    KalmanUpdater updater(0.1, 0.1, 0.1);
    Associator associator(predictor, updater, 0.1);
    Initiator initiator(associator, 4.0, 5.0, 5.0, 2.0, 1.0); // 初始化超时4s

    std::vector<Hypothesis> hypotheses;

    TrackedTargets tracked_targets;

    rd_float_t prior_state_vector[4] = { 1.0, 2.0, 3.0, 4.0 };
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
    tracked_target.life_cycle.unassociated_time = 2; // 2s未关联，大于初始化超时时间
    tracked_targets.push_back(tracked_target);


    // 列表里只有一个类
    printf("unassociated_time: %f\n", tracked_target.life_cycle.unassociated_time);
    printf("score: %d\n", tracked_target.life_cycle.score);
    std::cout << "\n----------------------------\n";
    initiator.update_lifecycle(tracked_targets, hypotheses);

    std::cout << "\n----------------------------\n";

    printf("unassociated_time: %f\n",
           tracked_targets.front().life_cycle.unassociated_time);     // 如果没有关联，则unassociated_time 加上 时间差，原始2加dt 1 结果为3，单位为s
    printf("unassociated_score: %d\n", initiator.unassociated_score); // unassociated_score = - initial score / unassociated time = -1000 / 4 = -250
    printf("score: %d\n", tracked_targets.front().life_cycle.score);

    EXPECT_EQ(tracked_targets.front().life_cycle.unassociated_time, 3); // 如果没有关联，则unassociated_time 加上 时间差，原始2加dt 1 结果为3，单位为s
    EXPECT_EQ(tracked_targets.front().life_cycle.score, -240); // 如果没有关联但未超时，变化score是-250*1，原本生命周期分数为10，加上变化score，结果为-240
}

TEST(RadarFixedTest3, update_lifecycle_associated_motion)
{
    KalmanPredictor predictor(0.1);
    KalmanUpdater updater(0.1, 0.1, 0.1);
    Associator associator(predictor, updater, 0.1);
    Initiator initiator(associator, 10.0, 5.0, 3.0, 2.0, 1.0); // 速度阈值2，keep_motion_score 5 ,keep_static_score 3，unassociated_time 5, unassociated_time 10

    std::vector<Hypothesis> hypotheses;

    TrackedTargets tracked_targets;

    rd_float_t prior_state_vector[4] = { 1.0, 2.0, 3.0, 4.0 };
    rd_float_t prior_state_covar[16] = { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };
    uint32_t timestamp_ms = 0;
    GaussianState prior_state(prior_state_vector, prior_state_covar, timestamp_ms);
    Hypothesis hypothesis(prior_state);
    hypothesis.prediction.timestamp_ms = 2000;
    hypothesis.prior_state.timestamp_ms = 1000;
    hypothesis.measurement = Vector3r(0, 0, 4); // 测试值是3维，phi，r，v，速度设为4，状态是运动状态
    hypothesis.has_meas = true;                 // 测量值不为空，关联
    hypotheses.push_back(hypothesis);

    TrackedTarget tracked_target(1, prior_state);
    tracked_target.life_cycle.score = 10;
    tracked_target.life_cycle.unassociated_time = 2; // 2s未关联，大于初始化超时时间
    tracked_targets.push_back(tracked_target);


    // 列表里只有一个类
    printf("unassociated_time: %f\n", tracked_target.life_cycle.unassociated_time);
    printf("score: %d\n", tracked_target.life_cycle.score);
    std::cout << "\n----------------------------\n";
    initiator.update_lifecycle(tracked_targets, hypotheses);

    std::cout << "\n----------------------------\n";

    printf("unassociated_time: %f\n",
           tracked_targets.front().life_cycle.unassociated_time);     // 如果没有关联，则unassociated_time 加上 时间差，原始2加dt 1 结果为3，单位为s
    printf("unassociated_score: %d\n", initiator.unassociated_score); // unassociated_score = - initial score / unassociated time = -1000 / 10 = -100
    printf("score: %d\n", tracked_targets.front().life_cycle.score);

    EXPECT_EQ(tracked_targets.front().life_cycle.unassociated_time, 0); // 如果没有关联，则unassociated_time 加上 时间差，原始2加dt 1 结果为3，单位为s
    EXPECT_EQ(tracked_targets.front().life_cycle.score,
              510); // 如果没有关联但未超时，变化score先减去了lifecycle.unassociated_time（2） * unassociated_score（-100） / 2，为100， ,motion_score
                    // 400，原本生命周期分数为10，加上变化score，结果为510
}

TEST(RadarFixedTest4, update_lifecycle_associated_static)
{
    KalmanPredictor predictor(0.1);
    KalmanUpdater updater(0.1, 0.1, 0.1);
    Associator associator(predictor, updater, 0.1);
    Initiator initiator(associator, 10.0, 5.0, 4.0, 5.0, 1.0); // 速度阈值5，keep_motion_score 5 ,keep_static_score 4，unassociated_time 5

    std::vector<Hypothesis> hypotheses;

    TrackedTargets tracked_targets;

    rd_float_t prior_state_vector[4] = { 1.0, 2.0, 3.0, 4.0 };
    rd_float_t prior_state_covar[16] = { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };
    uint32_t timestamp_ms = 0;
    GaussianState prior_state(prior_state_vector, prior_state_covar, timestamp_ms);
    Hypothesis hypothesis(prior_state);
    hypothesis.prediction.timestamp_ms = 2000;
    hypothesis.prior_state.timestamp_ms = 1000;
    hypothesis.measurement = Vector3r(0, 0, 4); // 测试值是3维，phi，r，v，速度设为4，状态是静止状态
    hypothesis.has_meas = true;                 // 测量值不为空，无法关联
    hypotheses.push_back(hypothesis);

    TrackedTarget tracked_target(1, prior_state);
    tracked_target.life_cycle.score = 10;
    tracked_target.life_cycle.unassociated_time = 2; // 2s未关联，大于初始化超时时间
    tracked_targets.push_back(tracked_target);


    // 列表里只有一个类
    printf("unassociated_time: %f\n", tracked_target.life_cycle.unassociated_time);
    printf("score: %d\n", tracked_target.life_cycle.score);
    std::cout << "\n----------------------------\n";
    initiator.update_lifecycle(tracked_targets, hypotheses);

    std::cout << "\n----------------------------\n";

    printf("unassociated_time: %f\n",
           tracked_targets.front().life_cycle.unassociated_time); // 如果没有关联，则unassociated_time 加上 时间差，原始2加dt 1 结果为3，单位为s
    printf("score: %d\n", tracked_targets.front().life_cycle.score);

    EXPECT_EQ(tracked_targets.front().life_cycle.unassociated_time, 0); // 如果没有关联，则unassociated_time 加上 时间差，原始2加dt 1 结果为3，单位为s
    EXPECT_EQ(tracked_targets.front().life_cycle.score,
              610); // 如果没有关联但未超时，变化score 100 ,stadic_score 500，原本生命周期分数为10，加上变化score，结果为610
}

// 测试 Initiator::create targetas函数
TEST(RadarFixedTest, creat_targets)
{

    static const uint32_t N = 4;
    static rd_float_t state_vector[N][4] = {
        { 4, 4, 1, 0 },
        { 4, 8, 6, 2 },
        { 1, 5, 7, 6 },
        { 0, 0, 7, 7 }
    };

    static rd_float_t meas[N][3] = {

    };

    for (int i = 0; i < N; i++) {
        rd_float_t *x = state_vector[i];
        rd_float_t *z = meas[i];
        z[0] = atan2(x[2], x[0]);
        z[1] = hypot(x[0], x[2]);
        z[2] = (x[0] * x[1] + x[2] * x[3]) / z[1];
    }

    // 定义一个存储 Vector3r 的向量
    std::vector<Vector3r> measurements;
    TrackedTargets unconfirmed_targets;
    uint32_t timestamp_ms = 1000;
    rd_float_t speed_threshold = 0.1;

    for (int i = 0; i < N; i++) {
        measurements.emplace_back((rd_float_t *)meas[i]);
    }

    printf("Measurements Info:\n");
    for (int i = 0; i < measurements.size(); i++) {
        printf("Speed %f\n", measurements[i](2));
    }
    // 创建 Initiator 对象并调用 creat_targets 函数
    KalmanPredictor predictor(0);
    KalmanUpdater updater(M_PI / 6, 6, 2.0);
    Associator associator(predictor, updater, 0.2);
    Initiator initiator(associator, 10, 2, 2, 0.1, 0.2);

    // 调用creat_targets
    initiator.creat_targets(unconfirmed_targets, measurements, timestamp_ms);

    printf("Measurements After:\n");
    for (const auto &target : unconfirmed_targets) {
        std::cout << std::left << "new UUID: " << target.uuid << std::endl;
        std::cout << std::left << "new state:\n " << target.state.state_vector << std::endl;
    }
}


// 测试 Initiator::move_confirmed_targets函数
TEST(RadarFixedTest, move_confirmed_targets)
{
    static int32_t targets_score[7] = { 3000, 1500, 5000, 100, 4500, 10, 3001 };
    TrackedTargets unconfirmed_targets; // 创建未确认目标
    TrackedTargets tracked_targets;     // 创建已确认目标
    for (int i = 0; i < 7; i++) {
        GaussianState state;
        TrackedTarget t(i, state);
        t.life_cycle.score = targets_score[i];
        unconfirmed_targets.emplace_back(t);
    }
    printf("unconfirmed_targets Info:\n");
    for (auto &t : unconfirmed_targets) {
        printf("UUID:%d Score %d\n", t.uuid, t.life_cycle.score);
    }
    // 创建 Initiator 对象并调用 creat_targets 函数
    KalmanPredictor predictor(0);
    KalmanUpdater updater(M_PI / 6, 6, 2.0);
    Associator associator(predictor, updater, 0.2);
    Initiator initiator(associator, 10, 2, 2, 0.1, 0.2);

    // 调用creat_targets
    initiator.move_confirmed_targets(tracked_targets, unconfirmed_targets);

    printf("tracked_targets After:\n");
    for (auto &t : tracked_targets) {
        printf("UUID:%d Score %d\n", t.uuid, t.life_cycle.score);
    }

    printf("unconfirmed_targets After:\n");
    for (auto &t : unconfirmed_targets) {
        printf("UUID:%d Score %d\n", t.uuid, t.life_cycle.score);
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

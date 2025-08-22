#include <radar/ot/track_associator.hh>
#include <radar/common/radar_log.h>
#include "rectangular_lsap.hh"
#include <cmath>
#include <vector>
#include <bitset>

static inline void move_unused_measurements(std::vector<Vector3r> &measurements, std::vector<int64_t> &used);


/**
 * @brief 根据目标列表初始化假设
 *
 * @param hypotheses
 * @param targets
 * @param timestamp_ms
 */
void Associator::hypotheses_init(std::vector<Hypothesis> &hypotheses, TrackedTargets &targets)
{
    // 清空并分配内存
    hypotheses.clear();
    size_t size = std::distance(targets.begin(), targets.end());
    hypotheses.reserve(size);

    // 初始化目标列表
    for (TrackedTarget &target : targets) {
        hypotheses.emplace_back(target.state);
    }

    return;
}

std::vector<Hypothesis> Associator::hypotheses_new(TrackedTargets &targets)
{
    size_t size = std::distance(targets.begin(), targets.end());
    std::vector<Hypothesis> hypotheses;
    hypotheses.reserve(size);
    for (TrackedTarget &target : targets) {
        hypotheses.emplace_back(target.state);
    }
    return hypotheses;
}

/**
 * @brief 数据关联
 *
 * @details 将假设中的先验状态和测量值进行关联，匹配结果保存到对应假设中；未使用的测量值保存在measurements头部
 *
 * @param[in,out]   hypotheses      假设；输入时假设中需要包含先验状态， 关联后将测量值和预测值保存在假设中
 * @param[in,out]   measurements    测量值；未关联的测量值会移动到measurements头部
 * @param           timestamp_ms    时间戳
 */
void Associator::associate(std::vector<Hypothesis> &hypotheses, std::vector<Vector3r> &measurements, uint32_t timestamp_ms, rd_float_t missed_distance)
{

    /* 预测状态向量： 将预测状态写入到假设中 */
    RD_DEBUG("[数据关联] 预测状态向量");
    for (Hypothesis &hypothesis : hypotheses) {
        predictor.predict(hypothesis.prediction, hypothesis.prior_state, timestamp_ms);
    }


    /* 预测测量值： 使用测量模型将预测状态转化成预测测量值，写入假设中 */
    RD_DEBUG("[数据关联] 预测测量值：");
    for (Hypothesis &hypothesis : hypotheses) {
        updater.predict_measurement(hypothesis.measurement_prediction, hypothesis.prediction);
        hypothesis.has_meas_pred = true;
    }


    /* 生成距离矩阵 */
    RD_DEBUG("[数据关联] 生成距离矩阵");
    size_t M = hypotheses.size(), N = measurements.size();
    std::vector<double> distance_matrix(M * (M + N), 100000);
    for (size_t i = 0; i < M; i++) {
        double *row = &distance_matrix[i * (N + M)];
        for (size_t j = 0; j < measurements.size(); j++) {
            row[j] = distance(hypotheses[i], measurements[j], 1, 1);
        }
    }
    for (size_t i = 1; i <= M; i++) {
        distance_matrix[i * (M + N) - i] = missed_distance;
    }


    /* GNN数据关联 */
    std::vector<int64_t> a(M), b(M);
    solve_rectangular_linear_sum_assignment(M, M + N, distance_matrix.data(), 0, a.data(), b.data());


    /* 将关联的测量值写入对应的假设 */
    for (int i = 0; i < M; i++) {
        if (b[i] < N) {
            Hypothesis &h = hypotheses[a[i]];
            h.set_measurement(measurements[b[i]]);
        }
    }

    /* 将未使用的测量值移动到measurements头部 */
    move_unused_measurements(measurements, b);


#if LOG_LEVEL <= LOG_LEVEL_DEBUG
    RADAR_LOG_PRINTF("[DEBUG]:\n");
    RADAR_LOG_PRINTF("距离矩阵 %dx%d:\n", M, M + N);
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < M + N; j++) {
            RADAR_LOG_PRINTF("%5.3f ", distance_matrix[i * (M + N) + j]);
        }
        RADAR_LOG_PRINTF("\n");
    }
    RADAR_LOG_PRINTF("\n");

    RADAR_LOG_PRINTF("关联结果：\n");
    for (size_t i = 0; i < M; i++) {
        RADAR_LOG_PRINTF("[%" PRId64 " %" PRId64 "]\n", a[i], b[i]);
    }
    RADAR_LOG_PRINTF("未使用的测量值:\n");
    for (int i = 0; i < measurements.size(); i++) {
        RADAR_LOG_PRINTF("%d:[%f %f %f]\n", i, measurements[i].x(), measurements[i].y(), measurements[i].z());
    }
#endif
    return;
};


static inline void move_unused_measurements(std::vector<Vector3r> &measurements, std::vector<int64_t> &used)
{
    size_t n = measurements.size();
    std::vector<bool> is_used(n, false); // 位集，初始化全为 false
    for (int64_t index : used) {
        if (index >= 0 && index < n) {
            is_used[index] = true;
        }
    }

    int i = 0;
    for (int j = 0; j < n; j++) {
        if (!is_used[j]) {
            if (i != j) {
                measurements[i] = measurements[j];
            }
            i++;
        }
    }
    measurements.resize(i);
}


/**
 * @brief  计算假设中的预测测量值和测量值之间的距离
 *
 * @param hypothesis 假设
 * @param measurement 测量值
 * @return rd_float_t
 */
rd_float_t Associator::distance(Hypothesis &hypothesis, Vector3r &measurement, rd_float_t wr, rd_float_t wv)
{
    Vector3r &a = hypothesis.measurement_prediction.state_vector;
    Vector3r &b = measurement;

    rd_float_t r0 = a[1];
    rd_float_t r1 = b[1];
    rd_float_t theta = a[0] - b[0];
    rd_float_t dr2 = r0 * r0 + r1 * r1 - 2.0f * r0 * r1 * cos(theta);
    rd_float_t dv = (a[2] - b[2]) * wv;
    rd_float_t dis = sqrt(dr2 * wr * wr + dv * dv);
    RD_DEBUG("原始坐标:[%f %f %f] [%f %f %f] 距离:%f\n", a[0], a[1], a[2], b[0], b[1], b[2], dis);
    return dis;
}

/**
 * @brief  更新器
 *
 * @param hypotheses 假设
 * @param targets 目标
 * @return 测量的预测值
 *
 * @attention targets 和 hypotheses 中的元素必须是一一对应的关系
 */
void Associator::update(TrackedTargets &targets, std::vector<Hypothesis> &hypotheses)
{
    RADAR_ASSERT_EQ((size_t)targets.size(), (size_t)hypotheses.size());

    // 对每一个目标执行卡尔曼更新
    std::vector<Hypothesis>::iterator h = hypotheses.begin();
    for (TrackedTarget &target : targets) {
        this->updater.update(target.state, *h);
        target.update_life_cycle_data(*h);
        h++;
    }

    return;
}

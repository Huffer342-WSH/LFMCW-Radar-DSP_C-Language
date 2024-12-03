/**
 * @file track_tracking.cc
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 跟踪器的C语言接口，内部C++实现
 * @version 0.1
 * @date 2024-12-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "track_tracking.hh"
#include "track_target.hh"


#include "radar_log.h"
#include <vector>
#include <algorithm>

#include <Eigen/Dense>


/**
 * @brief 目标跟踪
 *
 * @param tracked_targets       已跟踪的目标
 * @param unconfirmed_targets   未确认的目标
 * @param measurements          测量值
 * @param timestamp_ms          时间戳，单位毫秒(ms)
 */
void Tracker::track(TrackedTargets &tracked_targets, TrackedTargets &unconfirmed_targets, std::vector<Eigen::Vector3d> &measurements, uint32_t timestamp_ms)
{


    /* 航迹关联与航迹结束 */
    {
        RD_DEBUG("[目标跟踪] 初始化假设");
        /* 为每一个目标初始化假设 */
        std::vector<Hypothesis> hypotheses; // 在子代码块{} 1中的局部变量会在代码块结束后被销毁
        this->associator.hypotheses_init(hypotheses, tracked_targets, timestamp_ms);

        /* 数据关联 */
        RD_DEBUG("[目标跟踪] 数据关联");
        this->associator.associate(hypotheses, measurements, timestamp_ms);


        /* 滤波：更新器根据假设计算后验状态，并赋值给目标 */
        RD_DEBUG("[目标跟踪] 滤波");
        this->associator.update(tracked_targets, hypotheses);


        /* 航迹结束：删除器更新每一个目标的生命周期 */
        RD_DEBUG("[目标跟踪] 删除");
        this->deleter.delete_tracks(tracked_targets, hypotheses);
    }

    /* 航迹起始 */
    this->initiator.initiate(tracked_targets, unconfirmed_targets, measurements, timestamp_ms);

    return;
}


/**
 * @brief C语言接口
 *
 */
extern "C" {


/**
 * @brief  根据配置创建跟踪器
 *
 * @param   cfg                跟踪器配置
 * @return  tracker_handel_t*  跟踪器句柄，C语言端无法访问
 */
tracker_handel_t *tracker_new(tracker_config_t *cfg)
{
    Tracker *tracker = new Tracker(cfg);
    return reinterpret_cast<tracker_handel_t *>(tracker);
}


/**
 * @brief 运行跟踪器
 *
 * @param tracker_handel       跟踪器句柄
 * @param tracked_targets      跟踪状态目标的链表
 * @param unconfirmed_targets  起始状态目标的链表
 * @param measurements         测量值
 * @param timestamp            时间戳
 *
 * @details 输入一帧的测量值，内部完成已有目标的匹配和更新，以及新目标的创建等
 *          修改后的结果依然保存在tracked_targets和unconfirmed_targets中
 */
void tracker_run(tracker_handel_t *tracker_handel, tracked_targets_list_t *tracked_targets, tracked_targets_list_t *unconfirmed_targets,
                 measurements_t *measurements, uint32_t timestamp_ms)
{
    Tracker *tracker = reinterpret_cast<Tracker *>(tracker_handel);

    /* 转换测量值 */
    RD_DEBUG("初始化测量向量");
    size_t num_measurements = measurements->num;
    std::vector<Eigen::Vector3d> meas_vectors(num_measurements);
    for (size_t i = 0; i < num_measurements; i++) {
        measurement_t *m = &measurements->data[i];
        meas_vectors[i] << (double)m->azimuth / (8192.0), (double)m->distance / 1000.0, (double)m->velocity / 1000.0;
    }

    /* 转换跟踪目标 */
    TrackedTargets cxx_tracked_targets(tracked_targets);
    TrackedTargets cxx_unconfirmed_targets(unconfirmed_targets);


    RD_DEBUG("运行C++ 跟踪器");

    tracker->track(cxx_tracked_targets, cxx_unconfirmed_targets, meas_vectors, timestamp_ms);
}


} // extern "C"

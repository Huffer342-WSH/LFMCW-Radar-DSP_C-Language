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

#include <radar/ot/track_tracking.hh>
#include <radar/ot/track_target.hh>

#include <vector>
#include <algorithm>


/**
 * @brief 目标跟踪
 *
 * @param tracked_targets       已跟踪的目标
 * @param unconfirmed_targets   未确认的目标
 * @param measurements          测量值
 * @param timestamp_ms          时间戳，单位毫秒(ms)
 */
void Tracker::track(TrackedTargets &tracked_targets, TrackedTargets &unconfirmed_targets, std::vector<Vector3r> &measurements, uint32_t timestamp_ms)
{


    /* 航迹关联与航迹结束 */
    {
        RD_DEBUG("[目标跟踪] 初始化假设");
        /* 为每一个目标初始化假设 */
        std::vector<Hypothesis> hypotheses = this->associator.hypotheses_new(tracked_targets);

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


#if LOG_LEVEL <= LOG_LEVEL_INFO
    {

        static uint32_t frame_cnt = 0;
        frame_cnt++;
        RD_INFO("第%u帧>>>", frame_cnt);
        RADAR_LOG_PRINTF("未确定目标\n");
        for (auto target : unconfirmed_targets) {
            RADAR_LOG_PRINTF("ID:%" PRIu32 " X:[%f %f %f %f] T:%u Score:%d\n", target.uuid,
                target.state.state_vector(0), target.state.state_vector(1),
                target.state.state_vector(2), target.state.state_vector(3),
                target.state.timestamp_ms, target.life_cycle.score);
        }
        RADAR_LOG_PRINTF("\n已跟踪目标\n");
        for (auto target : tracked_targets) {
            RADAR_LOG_PRINTF("ID:%" PRIu32 " X:[%f %f %f %f] T:%u Score:%d\n", target.uuid,
                target.state.state_vector(0), target.state.state_vector(1),
                target.state.state_vector(2), target.state.state_vector(3),
                target.state.timestamp_ms, target.life_cycle.score);
        }
        RADAR_LOG_PRINTF("<<<\n\n");
    }
#endif

    return;
}

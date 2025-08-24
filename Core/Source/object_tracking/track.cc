#include <radar/ot/track.h>
#include <radar/ot/track_tracking.hh>

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
void tracker_run(tracker_handel_t *tracker_handel, tracked_targets_list_t *tracked_targets, tracked_targets_list_t *unconfirmed_targets, size_t meas_num,
                 uint32_t timestamp_ms, set_meas_callback cb, void *args)
{
    Tracker *tracker = reinterpret_cast<Tracker *>(tracker_handel);

    /* 转换测量值 */
    RD_DEBUG("初始化测量向量");
    tracker->measurements.resize(meas_num);
    cb(tracker->measurements.data()->data(), tracker->measurements.capacity(), args);

    /* 转换跟踪目标 */
    TrackedTargets *cxx_tracked_targets = reinterpret_cast<TrackedTargets *>(tracked_targets);
    TrackedTargets *cxx_unconfirmed_targets = reinterpret_cast<TrackedTargets *>(unconfirmed_targets);

    RD_DEBUG("运行C++ 跟踪器");

    tracker->track(*cxx_tracked_targets, *cxx_unconfirmed_targets, tracker->measurements, timestamp_ms);
}


} // extern "C"

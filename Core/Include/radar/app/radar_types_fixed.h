#ifndef _RADAR_TYPES_H_
#define _RADAR_TYPES_H_

#include <radar/common/radar_config.h>
#include <radar/common/radar_math_types.h>

#include <radar/sp/fixed_point/radar_cfar.h>
#include <radar/sp/fixed_point/radar_cluster.h>
#include <radar/sp/fixed_point/radar_clutter_filter.h>
#include <radar/sp/fixed_point/radar_measurement.h>
#include <radar/sp/fixed_point/radar_micromotion.h>

#include <radar/ot/track.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 雷达参数，主要包含指波形和采样等只读的参数
 */
typedef struct _radar_param_t {
    rd_float_t wavelength;      ///< 单位:m 雷达波长，24GHz雷达波长为 12.42663038e-3
    rd_float_t bandwidth;       ///< 单位:Hz 雷达有效带宽
    rd_float_t timeChirpPeriod; ///< 单位:s Chirp周期
    rd_float_t timeFramePeriod; ///< 单位:s 帧周期

    uint16_t numChannel;  ///< 雷达通道数
    uint16_t numSample;   ///< 采样点数
    uint16_t numRangeBin; ///< 距离单元数量
    uint16_t numChirp;    ///< Chirp数

    /* 以下参数位衍生参数，有上方参数计算得到，用于方便计算 */
    rd_float_t timeFrameDuration; ///< 单位:s 帧有效时长
    int32_t resRange;             ///< 单位:m 距离分辨率
    int32_t resVelocity;          ///< 单位:m/s 速度分辨率
    int32_t lambda_over_d_q15;    ///< 波长/天线间距,Q16.15定点数
} radar_param_t;

typedef struct {
    int32_t wr;
    int32_t wv;
    int32_t eps;
    size_t min_samples;
} dbscan_cfg_t;

/**
 * @brief 雷达配置，主要包含信号处理时可配置的参数，比如ROI、阈值等
 */
typedef struct {
    radar_clutter_filter_cfg_t clutter_filter_cfg;
    cfar2d_cfg_t cfarCfg;
    cfar2d_filter_cfg_t cfar_filter_cfg;
    int16_t channel_phase_diff_threshold; ///< 通道间相位差阈值，使用Q2.13弧度，取值范围[0,PI]
    int16_t channel_mag_diff_threshold;   ///< 通道间幅度差阈值，使用Q0.15定点数，取值范围[0,1)；
                                          ///< 并表示两个通道幅度的 abs(A-B) / (A+B)
    int32_t occlusion_radius;             ///< 遮挡半径，单位mm
    dbscan_cfg_t dbscan_cfg;
    tracker_config_t tracker_cfg;
} radar_config_t;

typedef struct {
    radar_param_t *param;
    matrix3d_complex_int16_t *rdms;
    matrix2d_int32_t *magSpec2D;
} radar_basic_data_t;

typedef struct {
    measurements_buffer_t *buffer;
    measurements_t *multi_frame_meas;
    size_t *multi_frame_meas_labels;
    measurements_t *cluster_meas;
    dbscan_neighbors_t *dbscan_handle;
} radar_cluster_t;

typedef struct {
    void (*hook_rdm)(const int16_t *rdm, size_t n_channel, size_t n_rb, size_t n_vb);
    void (*hook_cfar_raw)(const cfar2d_result_t *cfar);
    void (*hook_cfar_filtered)(const cfar2d_result_t *cfar);
    void (*hook_point_clouds)(const measurements_t *meas);
    void (*hook_point_clouds_filtered)(const measurements_t *meas);
    void (*hook_clusters)(const measurements_t *clusters);
    void (*hook_unconfirmed_targets)(const tracked_targets_list_t *targets);
    void (*hook_tracked_targets)(const tracked_targets_list_t *targets);
} radar_hook_t;

typedef struct {
    uint32_t cntFrame;                           ///< 帧计数器
    radar_param_t param;                         ///< 只读参数
    radar_config_t config;                       ///< 可配置参数
    radar_basic_data_t basic;                    ///< 基础数据（RDM、幅度谱等）
    radar_clutter_filter_t *clutter_filter;      ///< 雷达杂波滤波器
    radar_micromotion_handle_t micromotion;      ///< 微动信息
    cfar2d_result_t *cfar;                       ///< CFAR检测结果
    measurements_t *meas;                        ///< 量测值
    radar_cluster_t cluster;                     ///< 聚类结果
    radar_hook_t hook;                           ///< 钩子函数
    tracker_handel_t *tracker;                   ///< 航迹管理器
    tracked_targets_list_t *tracked_targets;     ///< 已跟踪目标列表
    tracked_targets_list_t *unconfirmed_targets; ///< 未确认目标列表
} radar_handle_t;

int radar_basic_data_init(radar_basic_data_t *basic, radar_param_t *param);
int radar_hook_init(radar_hook_t *hook);
int radar_cluster_init(radar_cluster_t *cluster, size_t num_frame, size_t num_meas,
                       size_t num_cluster);

void radar_basic_data_deinit(radar_basic_data_t *basic);
void radar_cluster_deinit(radar_cluster_t *cluster);

void radar_set_timeChirpPeriod(radar_handle_t *radar, float timeChirpPeriod);

#ifdef __cplusplus
}
#endif
#endif /* _RADAR_TYPES_H_ */

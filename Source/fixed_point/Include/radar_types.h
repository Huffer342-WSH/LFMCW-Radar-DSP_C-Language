#ifndef _RADAR_TYPES_H_
#define _RADAR_TYPES_H_

#include "radar_config.h"
#include "radar_math.h"
#include "radar_cfar.h"
#include "radar_measurement.h"
#include "radar_micromotion.h"
#include "radar_cluster.h"

/**
 * @brief 雷达参数，主要包含指波形和采样等只读的参数
 */
typedef struct {
    rd_float_t wavelength;   ///< 单位:m 雷达波长，24GHz雷达波长为 12.42663038e-3
    rd_float_t bandwidth;    ///< 单位:Hz 雷达有效带宽
    rd_float_t timeChrip;    ///< 单位:s
    rd_float_t timeChripGap; ///< 单位:s
    rd_float_t timeFrameGap; ///< 单位:s

    uint16_t numChannel;  ///< 雷达通道数
    uint16_t numSample;   ///< 采样点数
    uint16_t numRangeBin; ///< 距离单元数量
    uint16_t numChrip;    ///< Chrip数


    /* 以下参数位衍生参数，有上方参数计算得到，用于方便计算 */
    rd_float_t timeChripFull;
    rd_float_t timeFrameVaild; ///< 单位:s 一帧的有效时间
    rd_float_t timeFrameTotal; ///< 单位:s 一帧的有效时间
    int32_t resRange;          ///< 单位:m 距离分辨率
    int32_t resVelocity;       ///< 单位:m/s 速度分辨率
    int32_t lambda_over_d_q15; ///< 波长/天线间距,Q16.15定点数
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
    cfar2d_cfg_t cfarCfg;
    cfar2d_filter_cfg_t cfar_filter_cfg;
    int16_t channel_phase_diff_threshold; ///< 通道间相位差阈值，使用Q2.13弧度，取值范围[0,PI]
    int16_t channel_mag_diff_threshold;   ///< 通道间幅度差阈值，使用Q0.15定点数，取值范围[0,1)； 并表示两个通道幅度的 abs(A-B) / (A+B)
    int32_t occlusion_radius;             ///< 遮挡半径，单位mm
    dbscan_cfg_t dbscan_cfg;
} radar_config_t;

typedef struct {
    radar_param_t *param;
    matrix3d_complex_int16_t *rdms;
#if ENABLE_STATIC_CLUTTER_FILTERING == ON
    matrix2d_complex_int32_t *staticClutter;
    matrix2d_complex_int32_t *staticClutterAccBuffer;
#endif /* ENABLE_STATIC_CLUTTER_FILTERING == ON */
    matrix2d_int32_t *magSpec2D;
} radar_basic_data_t;

typedef struct {
    measurements_list_t *list;
    measurements_t *multi_frame_meas;
    size_t *multi_frame_meas_labels;
    measurements_t *cluster_meas;
} radar_cluster_t;


typedef struct {
    void (*hook_cfar_raw)(const cfar2d_result_t *cfar);
    void (*hook_cfar_filtered)(const cfar2d_result_t *cfar);
    void (*hook_point_clouds)(const measurements_t *meas);
    void (*hook_point_clouds_filtered)(const measurements_t *meas);
    void (*hook_clusters)(const measurements_t *clusters);
} radar_hook_t;

typedef struct {
    uint32_t cntFrame;
    radar_param_t param;
    radar_config_t config;
    radar_basic_data_t basic;
    radar_micromotion_handle_t micromotion;
    cfar2d_result_t *cfar;
    radar_cluster_t cluster;
    radar_hook_t hook;
} radar_handle_t;

#ifdef __cplusplus
extern "C" {
#endif

int radar_basic_data_init(radar_basic_data_t *basic, radar_param_t *param);
int radar_hook_init(radar_hook_t *hook);
int radar_cluster_init(radar_cluster_t *cluster, size_t num_frame, size_t num_meas, size_t num_cluster);

void radar_basic_data_deinit(radar_basic_data_t *basic);
void radar_cluster_deinit(radar_cluster_t *cluster);


#ifdef __cplusplus
}
#endif
#endif /* _RADAR_TYPES_H_ */

#ifndef _RADAR_TYPES_H_
#define _RADAR_TYPES_H_

#include "radar_math_types.h"
#include "radar_cfar.h"
#include "radar_micromotion.h"

/**
 * @brief 量测值，包含距离，速度，方位。CFAR检测结果作为补充
 *
 */
typedef struct {
    int32_t distance;    // 径向距离(mm)
    int32_t velocity;    // 径向速度(mm/s)
    int32_t azimuth;     // 方位角 (Q2.29)
    int32_t sin_azimuth; // sin(azimuth) Q0.31
    int32_t amp;
    int32_t snr;
} radar_measurements_fixed_t;

/**
 * @brief 量测值，包含距离，速度，方位。CFAR检测结果作为补充
 *
 */
typedef struct {
    radar_measurements_fixed_t *meas;
    size_t num;
    size_t capacity;
} radar_measurement_list_fixed_t;

/**
 * @brief 雷达参数，主要包含指波形和采样等只读的参数
 */
typedef struct {
    rd_float_t wavelength;   // 单位:m 雷达波长，24GHz雷达波长为 12.42663038e-3
    rd_float_t bandwidth;    // 单位:Hz 雷达有效带宽
    rd_float_t timeChrip;    // 单位:s
    rd_float_t timeChripGap; // 单位:s
    rd_float_t timeFrameGap; // 单位:s

    uint16_t numChannel;
    uint16_t numSample;
    uint16_t numRangeBin;
    uint16_t numChrip;

    /* 以下参数位衍生参数，有上方参数计算得到，用于方便计算 */
    rd_float_t timeFrameVaild; // 单位:s 一帧的有效时间
    rd_float_t timeFrameTotal; // 单位:s 一帧的有效时间
    int32_t resRange;          // 单位:m 距离分辨率
    int32_t resVelocity;       // 单位:m/s 速度分辨率

} radar_param_t;

/**
 * @brief 雷达配置，主要包含信号处理时可配置的参数，比如ROI、阈值等
 */
typedef struct {
    cfar2d_cfg_t cfarCfg;
    cfar2d_filter_cfg_t cfar_filter_cfg;
} radar_config_t;

typedef struct {
    radar_param_t *param;
    matrix3d_complex_int16_t *rdms;
    matrix2d_complex_int32_t *staticClutter;
    matrix2d_complex_int32_t *staticClutterAccBuffer;
    matrix2d_int32_t *magSpec2D;
} radar_basic_data_t;


typedef struct {
    uint32_t cntFrame;
    radar_param_t param;
    radar_config_t config;
    radar_basic_data_t basic;
    radar_micromotion_handle_t micromotion;
    cfar2d_result_t *cfar;
    radar_measurement_list_fixed_t *meas;
} radar_handle_t;

#ifdef __cplusplus
extern "C" {
#endif

int radar_basic_data_init(radar_basic_data_t *basic, radar_param_t *param);
radar_measurement_list_fixed_t *radar_measurement_list_alloc(size_t capacity);

#ifdef __cplusplus
}
#endif
#endif /* _RADAR_TYPES_H_ */

/**
 * @file radar_processer.h
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 雷达信号处理流程的具体实现
 * @version 0.1
 * @date 2024-11-20
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include <radar/app/radar_types_fixed.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    float wavelength;             ///< m, 波长
    float bandwidth;              ///< Hz, 带宽
    float rx_antenna_spacing;     ///< m, 接收天线间距
    float timeChirpPeriod;        ///< s, Chirp周期，两个Chirp头部之间的时长
    float timeFramePeriod;        ///< s, 帧周期，两个Frame头部之间的时长
    uint16_t numChannel;          ///< 雷达通道数
    uint16_t numRangeBin;         ///< 距离单元数
    uint16_t numChirp;            ///< Chirp数
    uint16_t numMaxCfarPoints;    ///< CFAR检测的最大点数，超出上限时较远距离的点会被丢弃
    uint16_t numMaxCachedFrame;   ///< 多帧叠加时，缓存的最大帧数
    uint16_t numMaxMeas;          ///< 多帧叠加时，缓存的量测值的最大数量
    uint16_t numMaxCluster;       ///< 聚类后簇的最大数量
} radar_init_param_t;


int radardsp_init(radar_handle_t *radar, radar_init_param_t *param, radar_config_t *config);

int radardsp_input_new_frame(radar_handle_t *radar, matrix3d_complex_int16_t *rdms, uint32_t timestamp_ms);

void radardsp_register_hook_cfar_raw(radar_handle_t *radar, void (*func)(const cfar2d_result_t *));
void radardsp_register_hook_cfar_filtered(radar_handle_t *radar, void (*func)(const cfar2d_result_t *));
void radardsp_register_hook_point_clouds(radar_handle_t *radar, void (*func)(const measurements_t *));
void radardsp_register_hook_point_clouds_filtered(radar_handle_t *radar, void (*func)(const measurements_t *));
void radardsp_register_hook_clusters(radar_handle_t *radar, void (*func)(const measurements_t *));


#ifdef __cplusplus
}
#endif

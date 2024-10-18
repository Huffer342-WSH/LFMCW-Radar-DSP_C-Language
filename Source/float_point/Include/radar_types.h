#ifndef _RADAR_TYPES_H_
#define _RADAR_TYPES_H_

#include <stdint.h>
#include "radar_math_types.h"

#ifdef __cplusplus
extern "C" {
#endif



/**
 * @brief 雷达参数，主要包含指波形和采样等只读的参数
 */
typedef struct {
    double wavelength;   //单位:m 雷达波长
    double bandwidth;    //单位:Hz 雷达有效带宽
    double timeChrip;    //单位:s
    double timeChripGap; //单位:s
    double timeFrameGap; //单位:s
    uint16_t numChannel;
    uint16_t numPoint;
    uint16_t numRangeBin;
    uint16_t numChrip;

    /* 以下参数位衍生参数，有上方参数计算得到，用于方便计算 */
    double timeFrameVaild; //单位:s 一帧的有效时间
    double resRange;       //单位:m 距离分辨率
    double resVelocity;    //单位:m/s 速度分辨率

} radar_param_t;

/**
 * @brief 雷达配置，主要包含信号处理时可配置的参数，比如ROI、阈值等
 */
typedef struct {
    uint16_t cfarGuardRange; //CFAR 保护单元大小-距离维度
    uint16_t cfarGuardVelo;  //CFAR 保护单元大小-速度维度
    uint16_t cfarTrainRange; //CFAR 训练单元大小-距离维度
    uint16_t cfarTrainVelo;  //CFAR 训练单元大小-速度维度
    double cfarThSNR;        //CFAR 阈值-信噪比
    double cfarThAmp;        //CFAR 阈值-幅度

} radar_config_t;

typedef struct {
    uint16_t numChannel;
    uint16_t numRangeBin;
    double *staticClutter;
} radar_basic_data_t;


typedef struct {
    double cntFrame;
    radar_param_t param;
    radar_config_t config;
    radar_basic_data_t basic;
} radar_handle_t;

#ifdef __cplusplus
}
#endif
#endif /* _RADAR_TYPES_H_ */

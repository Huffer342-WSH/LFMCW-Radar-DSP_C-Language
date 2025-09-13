#ifndef _RADARCONFIG_H_
#define _RADARCONFIG_H_

#include <stdio.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


extern void log_printf(const char *format, ...);
extern uint32_t radar_tick(void);

#define RADAR_LOG_PRINTF(format, ...) log_printf(format, ##__VA_ARGS__)

#define RADAR_TICK() radar_tick()

/* 静态杂波滤除 */
#define CONFIG_CLUTTER_FILTER

/* 内部计算幅度谱还是外部直接提供 */
#define AMPLITUDE_SPECTRUM_CALCULATION_METHOD AMP_SPEC_CLAC_METHOD_INSIDE

/* 浮点数数据类型 */
#define FLOAT_TYPE double

/* 封装malloc/free, 在rd_malloc/rd_calloc/rd_free中添加额外的日志 */
// #define CONFIG_MM_WRAP


#ifdef __cplusplus
}
#endif
#endif /* _RADARCONFIG_H_ */

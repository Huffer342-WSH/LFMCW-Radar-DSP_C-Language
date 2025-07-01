#ifndef _RADARCONFIG_H_
#define _RADARCONFIG_H_

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 日志 ----------------------------------------*/

static inline void log_printf(const char *format, ...)
{
    FILE *logfile = fopen("app.log", "a");
    if (logfile != NULL) {
        va_list args;
        va_start(args, format);
        vfprintf(logfile, format, args);
        va_end(args);
        fclose(logfile);
    } else {
        fprintf(stderr, "Unable to open log file");
    }
}
#define RADAR_LOG_PRINTF(format, ...) \
    printf(format, ##__VA_ARGS__);    \
    log_printf(format, ##__VA_ARGS__)

/* 时间戳 ----------------------------------------*/
#ifdef _WIN32

#include <windows.h>
#define RADAR_TICK()                                                        \
    ({                                                                      \
        LARGE_INTEGER counter, freq;                                        \
        QueryPerformanceCounter(&counter);                                  \
        QueryPerformanceFrequency(&freq);                                   \
        (uint32_t)((counter.QuadPart * 1000 / freq.QuadPart) & 0xFFFFFFFF); \
    })

#else

#include <sys/time.h>
#define RADAR_TICK()                                                       \
    ({                                                                     \
        struct timeval tv;                                                 \
        gettimeofday(&tv, NULL);                                           \
        (uint32_t)((tv.tv_sec * 1000UL + tv.tv_usec / 1000) & 0xFFFFFFFF); \
    })

#endif


/* 功能 ----------------------------------------*/

/* 静态杂波滤除 */
#define ENABLE_STATIC_CLUTTER_FILTERING OFF

/* 内部计算幅度谱还是外部直接提供 */
#define AMPLITUDE_SPECTRUM_CALCULATION_METHOD AMP_SPEC_CLAC_METHOD_INSIDE

/* 浮点数数据类型 */
#define FLOAT_TYPE double

/* MM封装 malloc/free
 *
 * 在rd_malloc/rd_calloc/rd_free中添加额外的日志
 */
#define CONFIG_MM_WRAP

#ifdef __cplusplus
}
#endif
#endif /* _RADARCONFIG_H_ */

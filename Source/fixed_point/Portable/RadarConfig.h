#ifndef _RADARCONFIG_H_
#define _RADARCONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* 日志 ----------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
static inline void log_printf(const char *format, ...)
{
    // 打开文件（追加模式）
    FILE *logfile = fopen("app.log", "a");
    if (logfile != NULL) {
        va_list args;
        va_start(args, format);

        // 打印日志到文件
        vfprintf(logfile, format, args);

        va_end(args);
        fclose(logfile);
    } else {
        fprintf(stderr, "Unable to open log file");
    }
}
#define RADAR_LOG_PRINTF(format, ...) log_printf(format, ##__VA_ARGS__)

/* 功能 ----------------------------------------*/

/* 静态杂波滤除 */
#define ENABLE_STATIC_CLUTTER_FILTERING OFF

/* 内部计算幅度谱还是外部直接提供 */
#define AMPLITUDE_SPECTRUM_CALCULATION_METHOD AMP_SPEC_CLAC_METHOD_INSIDE


#ifdef __cplusplus
}
#endif
#endif /* _RADARCONFIG_H_ */

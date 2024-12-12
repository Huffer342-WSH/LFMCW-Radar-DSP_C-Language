#pragma once

#include <stdarg.h>


/**
 * @brief  默认的日志打印函数
 */
#ifndef RADAR_LOG_PRINTF

#include <stdio.h>

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

#endif /* RADAR_LOG_PRINTF */

// 定义日志等级
#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_ERROR 3
#define LOG_LEVEL_ASSERT 4


// 当前启用的日志等级
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DEBUG
#endif


#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define RD_LOG(prefix, format, ...) RADAR_LOG_PRINTF("[%s] [%s:%d]\n>>> " format "\r\n", prefix, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define RD_LOG(prefix, format, ...)
#endif


#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define RD_DEBUG(format, ...) RD_LOG("DEBUG", format, ##__VA_ARGS__)
#else
#define RD_DEBUG(format, ...)
#endif


#if LOG_LEVEL <= LOG_LEVEL_INFO
#define RD_INFO(format, ...) RD_LOG("INFO", format, ##__VA_ARGS__)
#else
#define RD_INFO(format, ...)
#endif


#if LOG_LEVEL <= LOG_LEVEL_WARN
#define RD_WARN(format, ...) RD_LOG("WARN", format, ##__VA_ARGS__)
#else
#define RD_WARN(format, ...)
#endif


#if LOG_LEVEL <= LOG_LEVEL_ERROR
#define RD_ERROR(format, ...) RD_LOG("ERROR", format, ##__VA_ARGS__)
#else
#define RD_ERROR(format, ...)
#endif


#if LOG_LEVEL <= LOG_LEVEL_ASSERT
#define RD_ASSERT(format, ...) RD_LOG("ASSERT FAILED !!!", format, ##__VA_ARGS__)
#else
#define RD_ASSERT(format, ...)
#endif


#include "radar_assert.h"
#include "radar_error.h"

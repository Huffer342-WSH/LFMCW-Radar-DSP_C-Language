#pragma once

#ifndef __RADAR_LOG_H__
#define __RADAR_LOG_H__

#include <radar/common/radar_config.h>

#include <stdarg.h>


/**
 * @brief  默认的日志打印函数
 */
#ifndef RADAR_LOG_PRINTF
#warning RADAR_LOG_PRINTF is not defined

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


#ifndef FORCE_ENABLE_LOG
#define FORCE_ENABLE_LOG 0
#endif

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define RD_PRINTF(format, ...) RADAR_LOG_PRINTF(format, ##__VA_ARGS__)
#else
#define RD_PRINTF(format, ...)
#endif


#if LOG_LEVEL <= LOG_LEVEL_ASSERT
#ifdef LOG_NO_PREFIX
#define RD_LOG(prefix, format, ...) RADAR_LOG_PRINTF(format, ##__VA_ARGS__)
#else
#define RD_LOG(prefix, format, ...) RADAR_LOG_PRINTF("[%s] [%s:%d]>>>\n" format "\r\n", prefix, __FILE__, __LINE__, ##__VA_ARGS__)
#endif
#endif


#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define RD_DEBUG(format, ...) RD_LOG("DEBUG", format, ##__VA_ARGS__)
#else
#define RD_DEBUG(format, ...) \
    if (FORCE_ENABLE_LOG)     \
    RD_LOG("DEBUG", format, ##__VA_ARGS__)
#endif


#if LOG_LEVEL <= LOG_LEVEL_INFO
#define RD_INFO(format, ...) RD_LOG("INFO", format, ##__VA_ARGS__)
#else
#define RD_INFO(format, ...) \
    if (FORCE_ENABLE_LOG)    \
    RD_LOG("INFO", format, ##__VA_ARGS__)
#endif


#if LOG_LEVEL <= LOG_LEVEL_WARN
#define RD_WARN(format, ...) RD_LOG("WARN", format, ##__VA_ARGS__)
#else
#define RD_WARN(format, ...) \
    if (FORCE_ENABLE_LOG)    \
    RD_LOG("WARN", format, ##__VA_ARGS__)
#endif


#if LOG_LEVEL <= LOG_LEVEL_ERROR
#define RD_ERROR(format, ...) RD_LOG("ERROR", format, ##__VA_ARGS__)
#else
#define RD_ERROR(format, ...) \
    if (FORCE_ENABLE_LOG)     \
    RD_LOG("ERROR", format, ##__VA_ARGS__)
#endif


#if LOG_LEVEL <= LOG_LEVEL_ASSERT
#define RD_ASSERT(format, ...) RD_LOG("ASSERT FAILED !!!", format, ##__VA_ARGS__)
#else
#define RD_ASSERT(format, ...) \
    if (FORCE_ENABLE_LOG)      \
    RD_LOG("ASSERT FAILED !!!", format, ##__VA_ARGS__)
#endif


#include "radar_assert.h"
#include "radar_error.h"

#endif

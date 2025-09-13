#pragma once

#ifndef __RADAR_LOG_H__
#error "Just include radar_log.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

enum {
    OK = 0,          ///< 成功
    RADAR_ENOMEM,    ///< 内存分配失败
    RADAR_ECAPACITY, ///< 容量不足
    RADAR_EFREENULL, ///< 释放空指针
    RADAR_EOVRFLW,   ///< 超出范围
    RADAR_EINVAL,    ///< 参数无效
    RADAR_EOCCUPIED, ///< 资源被占用
};

#define RADAR_ERROR(reason, error_code) radar_error(reason, __FILE__, __LINE__, error_code);


void radar_error(const char *reason, const char *file, int line, int error_code);


#ifdef __cplusplus
}
#endif

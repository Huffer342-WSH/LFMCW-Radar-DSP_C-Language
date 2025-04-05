#pragma once

#ifndef __RADAR_LOG_H__
#error "Just include radar_log.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif


enum {
    RADAR_SUCCESS = 0,
    RADAR_ENOMEM,
    RADAR_EFREENULL,
    RADAR_EOVRFLW

};


#define RADAR_ERROR(reason, error_code) radar_error(reason, __FILE__, __LINE__, error_code);


void radar_error(const char *reason, const char *file, int line, int error_code);


#ifdef __cplusplus
}
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <windows.h>

void log_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    vprintf(format, args);

    FILE *logfile = fopen("app.log", "a");
    if (logfile != NULL) {

        vfprintf(logfile, format, args);
        fclose(logfile);
    } else {
        fprintf(stderr, "Unable to open log file");
    }

    va_end(args);
}

uint32_t radar_tick(void)
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);

    ULONGLONG t = (((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime);

    const ULONGLONG EPOCH_DIFF = 116444736000000000ULL; // 1601→1970 偏移 (100ns 单位)
    ULONGLONG unix_ms = (t - EPOCH_DIFF) / 10000ULL;

    return (uint32_t)(unix_ms & 0xFFFFFFFFULL);
}
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <time.h>

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
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    uint64_t ms = (uint64_t)ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;
    return (uint32_t)(ms & 0xFFFFFFFFU);
}

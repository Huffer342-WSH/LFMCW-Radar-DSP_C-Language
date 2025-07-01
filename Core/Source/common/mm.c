#define _MM_C_

#include <stdlib.h>
#include <stdio.h>
#include <radar/common/radar_log.h>
#include <radar/common/time.h>

void *_rd_malloc(size_t size, const char *file, int line, const char *func)
{
    void *ptr;
    uint32_t tick = get_tick();
    ptr = malloc(size);
    if (ptr) {
        RADAR_LOG_PRINTF("MM:t=%u alloc p=[0x%ju] sz=%ju @ %s:%d %s\n", tick, ptr, size, file, line, func);
    } else {
        RD_ERROR("alloc %ju failed\n", size);
    }

    return ptr;
}


void *_rd_calloc(size_t n, size_t size, const char *file, int line, const char *func)
{
    void *ptr;
    uint32_t tick = get_tick();
    ptr = calloc(n, size);
    if (ptr) {
        RADAR_LOG_PRINTF("MM:t=%u alloc p=[0x%ju] sz=%ju @ %s:%d %s\n", tick, ptr, n * size, file, line, func);
    } else {
        RD_ERROR("alloc %ju failed\n", n * size);
    }
    return ptr;
}

void _rd_free(void *ptr, const char *file, int line, const char *func)
{
    free(ptr);
    uint32_t tick = get_tick();
    RADAR_LOG_PRINTF("MM:t=%u free p=[0x%ju] @ %s:%d %s\n", tick, ptr, file, line, func);
}

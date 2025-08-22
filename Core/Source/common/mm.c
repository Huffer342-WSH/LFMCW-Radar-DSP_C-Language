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
        RADAR_LOG_PRINTF("MM:t=%u alloc p=[0x%" PRIxPTR "] sz=%zu @ %s:%d %s\n", tick,
            (uintptr_t)ptr, size, file, line, func);
    } else {
        RD_ERROR("alloc %zu failed\n", size);
    }

    return ptr;
}


void *_rd_calloc(size_t n, size_t size, const char *file, int line, const char *func)
{
    void *ptr;
    uint32_t tick = get_tick();
    ptr = calloc(n, size);
    if (ptr) {
        RADAR_LOG_PRINTF("MM:t=%u alloc p=[0x%" PRIxPTR "] sz=%zu @ %s:%d %s\n", tick,
            (uintptr_t)ptr, n * size, file, line, func);
    } else {
        RD_ERROR("alloc %zu failed\n", n * size);
    }
    return ptr;
}

void _rd_free(void *ptr, const char *file, int line, const char *func)
{
    free(ptr);
    uint32_t tick = get_tick();
    RADAR_LOG_PRINTF(
        "MM:t=%u free p=[0x%" PRIxPTR "] @ %s:%d %s\n", tick, (uintptr_t)ptr, file, line, func);
}

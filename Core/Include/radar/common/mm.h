#pragma once

#include <stdint.h>
#include "RadarConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_MM_WRAP

#define rd_malloc(size) _rd_malloc(size, __FILE__, __LINE__, __func__)
#define rd_calloc(n, size) _rd_calloc(n, size, __FILE__, __LINE__, __func__)
#define rd_free(ptr) _rd_free(ptr, __FILE__, __LINE__, __func__)

void *_rd_malloc(size_t size, const char *file, int line, const char *func);
void *_rd_calloc(size_t n, size_t size, const char *file, int line, const char *func);
void _rd_free(void *ptr, const char *file, int line, const char *func);

#else

#define rd_malloc(size) malloc(size)
#define rd_calloc(n, size) calloc(n, size)
#define rd_free(ptr) free(ptr)

#endif

#ifdef __cplusplus
}
#endif

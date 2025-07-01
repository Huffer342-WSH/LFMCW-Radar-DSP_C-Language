#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <radar/common/mm.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef uintptr_t elm;

#if UINTPTR_MAX == UINT64_MAX
#define elm_shift 6
#elif UINTPTR_MAX == UINT32_MAX
#define elm_shift 5
#endif


typedef struct bitset {
    size_t size;
    elm data[];
} bitset_t;

#define ELM_BYTES (sizeof(elm))
#define ELM_BITS (sizeof(elm) * 8)
#define ELM_BITS_MASK (sizeof(elm) * 8 - 1)

#define _bits2words(n) ((n + ELM_BITS_MASK) >> elm_shift)
#define _bits2bytes(n) (_bits2words(n) * ELM_BYTES)


static inline bitset_t *bitset_new(size_t n, bool value)
{
    size_t num_elm = _bits2words(n);
    bitset_t *self = (bitset_t *)rd_malloc(sizeof(size_t) + num_elm * sizeof(elm));
    memset(self->data, value ? ~0 : 0, num_elm * sizeof(elm));
    return self;
}


static inline void bitset_set(bitset_t *self, size_t pos)
{
    self->data[pos >> elm_shift] |= ((elm)1 << (pos & ELM_BITS_MASK));
}


static inline void bitset_reset(bitset_t *self, size_t pos)
{
    self->data[pos >> elm_shift] &= ~((elm)1 << (pos & ELM_BITS_MASK));
}


static inline bool bitset_test(bitset_t *self, size_t pos)
{
    return (self->data[pos >> elm_shift] & ((elm)1 << (pos & ELM_BITS_MASK))) ? true : false;
}

static inline void bitset_delete(bitset_t *self)
{
    rd_free(self);
}


#ifdef __cplusplus
}
#endif

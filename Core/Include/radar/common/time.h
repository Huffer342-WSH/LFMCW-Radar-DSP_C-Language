#include "RadarConfig.h"
#include <stdint.h>

#ifdef RADAR_TICK

static inline uint32_t get_tick(void)
{
    uint32_t tick;
    tick = RADAR_TICK();
    return tick;
}

#endif

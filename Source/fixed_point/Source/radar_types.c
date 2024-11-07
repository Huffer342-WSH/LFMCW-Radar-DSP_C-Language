#include "radar_types.h"

#include <stdlib.h>

radar_measurement_list_t *radar_measurement_list_alloc(size_t capacity)
{
    radar_measurement_list_t *meas = (radar_measurement_list_t *)malloc(sizeof(radar_measurement_list_t));
    meas->meas = (radar_measurements_t *)malloc(sizeof(radar_measurements_t) * capacity);
    meas->num = 0;
    meas->capacity = capacity;
    return meas;
}

#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#include "radar_math.h"
#include "radar_types.h"


typedef size_t *(*cb_get_neighbors_t)(size_t *count, size_t idx, int32_t eps, void *param);

void dbscan_core(size_t *labels, size_t n, int32_t eps, size_t min_samples, cb_get_neighbors_t get_neighbors, void *param);

int32_t *radar_cluster_calc_distance(radar_measurement_list_fixed_t *meas, int32_t wr, int32_t wv);

#ifdef __cplusplus
}
#endif

#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#include "radar_math.h"
#include "radar_types.h"


typedef struct {
    size_t n;
    int32_t *D;
    size_t **neighborhoods;
    size_t *n_neighbors;
} dbscan_neighbors_t;


typedef size_t *(*cb_get_neighbors_t)(size_t *count, size_t idx, int32_t eps, void *param);

void dbscan_core(size_t *labels, size_t n, int32_t eps, size_t min_samples, cb_get_neighbors_t get_neighbors, void *param);

int32_t radar_cluster_meas_distance(radar_measurements_fixed_t *ma, radar_measurements_fixed_t *mb, int32_t wr, int32_t wv);

dbscan_neighbors_t *radar_cluster_dbscan_neighbors_create(radar_measurement_list_fixed_t *meas, int32_t wr, int32_t wv, int32_t eps);
void radar_cluster_dbscan_neighbors_free(dbscan_neighbors_t *nb);

void radar_cluster_dbscan(size_t *labels, radar_measurement_list_fixed_t *meas, int32_t wr, int32_t wv, int32_t eps, size_t min_samples);
#ifdef __cplusplus
}
#endif

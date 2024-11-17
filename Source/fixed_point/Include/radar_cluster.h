#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "radar_math.h"
#include "radar_measurement.h"

typedef struct {
    size_t n;
    int32_t *D;
    size_t **neighborhoods;
    size_t *n_neighbors;
} dbscan_neighbors_t;

typedef struct {
    int32_t wr;
    int32_t wv;
    int32_t eps;
    size_t min_samples;
} dbscan_cfg_t;


typedef size_t *(*cb_get_neighbors_t)(size_t *count, size_t idx, int32_t eps, void *param);

int dbscan_core(size_t *labels, size_t n, int32_t eps, size_t min_samples, cb_get_neighbors_t get_neighbors, void *param);

int32_t radar_cluster_meas_distance(measurement_t *ma, measurement_t *mb, int32_t wr, int32_t wv);

dbscan_neighbors_t *radar_cluster_dbscan_neighbors_create(measurements_t *meas, int32_t wr, int32_t wv, int32_t eps);
void radar_cluster_dbscan_neighbors_free(dbscan_neighbors_t *nb);

int radar_cluster_dbscan(size_t *labels, measurements_t *meas, int32_t wr, int32_t wv, int32_t eps, size_t min_samples);

int radar_cluster_fusion(measurements_t *clusters, size_t num_cluster, size_t *labels, measurements_t *meas);

#ifdef __cplusplus
}
#endif

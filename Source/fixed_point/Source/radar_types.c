#include "radar_types.h"
#include "radar_error.h"

#include <stdlib.h>


int radar_basic_data_init(radar_basic_data_t *basic, radar_param_t *param)
{
    basic->param = param;
    basic->rdms = NULL; //  rdm由外部输入，不需要内部分配

#if ENABLE_STATIC_CLUTTER_FILTERING == ON
    basic->staticClutter = radar_matrix2d_complex_int32_alloc(param->numChannel, param->numRangeBin);
    basic->staticClutterAccBuffer = radar_matrix2d_complex_int32_alloc(param->numChannel, param->numRangeBin);
#endif
    basic->magSpec2D = radar_matrix2d_int32_alloc(param->numRangeBin, param->numChrip);
    if (basic->magSpec2D == NULL) {
        RADAR_ERROR("failed to allocate space for magSpec2D", RADAR_ENOMEM);
        return -1;
    }
    return 0;
}


int radar_hook_init(radar_hook_t *hook)
{
    hook->hook_cfar_raw = NULL;
    hook->hook_cfar_filtered = NULL;
    hook->hook_point_clouds = NULL;
    hook->hook_point_clouds_filtered = NULL;
    hook->hook_clusters = NULL;
    return 0;
}

int radar_cluster_init(radar_cluster_t *cluster, size_t num_frame, size_t num_meas, size_t num_cluster)
{
    int status = 0;
    cluster->list = radar_measurements_list_alloc(num_frame);
    if (cluster->list == NULL) {
        RADAR_ERROR("radar_cluster_init() failed to allocate space for measurements list", RADAR_ENOMEM);
        status = -1;
        goto RADAR_CLUSTER_INIT_FAILED1;
    }

    cluster->multi_frame_meas = radar_measurements_alloc(num_meas);
    if (cluster->multi_frame_meas == NULL) {
        RADAR_ERROR("radar_cluster_init() failed to allocate space for measurements", RADAR_ENOMEM);
        status = -2;
        goto RADAR_CLUSTER_INIT_FAILED2;
    }

    cluster->multi_frame_meas_labels = malloc(sizeof(size_t) * num_meas);
    if (cluster->multi_frame_meas_labels == NULL) {
        RADAR_ERROR("radar_cluster_init() failed to allocate space for measurements labels", RADAR_ENOMEM);
        status = -3;
        goto RADAR_CLUSTER_INIT_FAILED3;
    }

    cluster->cluster_meas = radar_measurements_alloc(num_cluster);
    if (cluster->cluster_meas == NULL) {
        RADAR_ERROR("radar_cluster_init() failed to allocate space for cluster measurements", RADAR_ENOMEM);
        status = -4;
        goto RADAR_CLUSTER_INIT_FAILED4;
    }


    return 0;

RADAR_CLUSTER_INIT_FAILED4:
    free(cluster->multi_frame_meas_labels);
RADAR_CLUSTER_INIT_FAILED3:
    radar_measurements_free(cluster->multi_frame_meas);
RADAR_CLUSTER_INIT_FAILED2:
    radar_measurements_list_free(cluster->list);
RADAR_CLUSTER_INIT_FAILED1:
    return status;
}

void radar_basic_data_deinit(radar_basic_data_t *basic)
{
    radar_matrix2d_int32_free(basic->magSpec2D);
#if ENABLE_STATIC_CLUTTER_FILTERING == ON
    radar_matrix2d_complex_int32_free(basic->staticClutter);
    radar_matrix2d_complex_int32_free(basic->staticClutterAccBuffer);
#endif
    return;
}

void radar_cluster_deinit(radar_cluster_t *cluster)
{
    free(cluster->multi_frame_meas_labels);
    radar_measurements_free(cluster->multi_frame_meas);
    radar_measurements_free(cluster->cluster_meas);
    radar_measurements_list_free(cluster->list);
    return;
}


void default_hook_cfar_raw(const cfar2d_result_t *cfar)
{
    (void)cfar;
}


void default_hook_cfar_filtered(const cfar2d_result_t *cfar)
{
    (void)cfar;
}

void default_hook_point_clouds(const measurements_t *meas)
{
    (void)meas;
}


void default_hook_clusters(const measurements_t *clusters)
{
    (void)clusters;
}

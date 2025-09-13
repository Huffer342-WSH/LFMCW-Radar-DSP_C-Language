#include <radar/app/radar_types_fixed.h>
#include <radar/common/radar_log.h>
#include <radar/common/mm.h>

#include <string.h>

int radar_basic_data_init(radar_basic_data_t *basic, radar_param_t *param)
{
    basic->param = param;
    basic->rdms = NULL; //  rdm由外部输入，不需要内部分配
    basic->magSpec2D = radar_matrix2d_int32_alloc(param->numRangeBin, param->numChirp);
    if (basic->magSpec2D == NULL) {
        RADAR_ERROR("failed to allocate space for magSpec2D", RADAR_ENOMEM);
        return -1;
    }
    return 0;
}

int radar_hook_init(radar_hook_t *hook)
{
    memset(hook, 0, sizeof(radar_hook_t));
    return 0;
}

int radar_cluster_init(radar_cluster_t *cluster, size_t num_frame, size_t num_meas, size_t num_cluster)
{
    int status = 0;

    memset(cluster, 0, sizeof(radar_cluster_t));

    /* 分配 measurements buffer */
    cluster->buffer = radar_measurements_buffer_alloc(num_frame + 1, num_meas + 1);
    if (cluster->buffer == NULL) {
        RADAR_ERROR("radar_cluster_init() failed to allocate measurements buffer", RADAR_ENOMEM);
        status = -1;
        goto errout;
    }

    /* 分配 multi_frame_meas */
    cluster->multi_frame_meas = radar_measurements_alloc(num_meas);
    if (cluster->multi_frame_meas == NULL) {
        RADAR_ERROR(
            "radar_cluster_init() failed to allocate multi_frame measurements", RADAR_ENOMEM);
        status = -2;
        goto errout;
    }

    /* 分配 multi_frame_meas_labels */
    cluster->multi_frame_meas_labels = rd_malloc(sizeof(size_t) * num_meas);
    if (cluster->multi_frame_meas_labels == NULL) {
        RADAR_ERROR("radar_cluster_init() failed to allocate measurements labels", RADAR_ENOMEM);
        status = -3;
        goto errout;
    }

    /* 分配 cluster_meas */
    cluster->cluster_meas = radar_measurements_alloc(num_cluster);
    if (cluster->cluster_meas == NULL) {
        RADAR_ERROR("radar_cluster_init() failed to allocate cluster measurements", RADAR_ENOMEM);
        status = -4;
        goto errout;
    }

    /* 分配DBSCAN的邻居信息 */
    cluster->dbscan_handle = radar_cluster_dbscan_neighbors_alloc(num_meas);
    if (cluster->dbscan_handle == NULL) {
        RADAR_ERROR("radar_cluster_init() failed to allocate dbscan neighbors", RADAR_ENOMEM);
        status = -5;
        goto errout;
    }

    return 0;

errout:
    radar_cluster_deinit(cluster);

    return status;
}


void radar_basic_data_deinit(radar_basic_data_t *basic)
{
    radar_matrix2d_int32_free(basic->magSpec2D);
    return;
}

void radar_cluster_deinit(radar_cluster_t *cluster)
{
    if (cluster->cluster_meas)
        radar_measurements_free(cluster->cluster_meas);
    if (cluster->multi_frame_meas_labels)
        rd_free(cluster->multi_frame_meas_labels);
    if (cluster->multi_frame_meas)
        radar_measurements_free(cluster->multi_frame_meas);
    if (cluster->buffer)
        radar_measurements_buffer_free(cluster->buffer);
    if (cluster->dbscan_handle)
        radar_cluster_dbscan_neighbors_free(cluster->dbscan_handle);

    memset(cluster, 0, sizeof(radar_cluster_t));
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


void radar_set_timeChirpPeriod(radar_handle_t *radar, float timeChirpPeriod)
{
    radar->param.timeChirpPeriod = timeChirpPeriod;
    radar->param.timeFrameDuration = radar->param.numChirp * radar->param.timeChirpPeriod;
    radar->param.resVelocity = radar->param.wavelength / (2 * radar->param.timeFrameDuration) * 1000;
}

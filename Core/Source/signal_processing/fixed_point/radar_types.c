#include <radar/app/radar_types_fixed.h>
#include <radar/common/radar_log.h>
#include <radar/common/mm.h>


int radar_basic_data_init(radar_basic_data_t *basic, radar_param_t *param)
{
    basic->param = param;
    basic->rdms = NULL; //  rdm由外部输入，不需要内部分配

#if ENABLE_STATIC_CLUTTER_FILTERING == ON
    basic->staticClutter = radar_matrix2d_complex_int32_alloc(param->numChannel, param->numRangeBin);
    basic->staticClutterAccBuffer = radar_matrix2d_complex_int32_alloc(param->numChannel, param->numRangeBin);
#endif
    basic->magSpec2D = radar_matrix2d_int32_alloc(param->numRangeBin, param->numChirp);
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

    cluster->buffer = NULL;
    cluster->multi_frame_meas = NULL;
    cluster->multi_frame_meas_labels = NULL;
    cluster->cluster_meas = NULL;

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

    return 0;

errout:
    if (cluster->cluster_meas) {
        radar_measurements_free(cluster->cluster_meas);
        cluster->cluster_meas = NULL;
    }
    if (cluster->multi_frame_meas_labels) {
        rd_free(cluster->multi_frame_meas_labels);
        cluster->multi_frame_meas_labels = NULL;
    }
    if (cluster->multi_frame_meas) {
        radar_measurements_free(cluster->multi_frame_meas);
        cluster->multi_frame_meas = NULL;
    }
    if (cluster->buffer) {
        radar_measurements_buffer_free(cluster->buffer);
        cluster->buffer = NULL;
    }

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
    rd_free(cluster->multi_frame_meas_labels);
    radar_measurements_free(cluster->multi_frame_meas);
    radar_measurements_free(cluster->cluster_meas);
    radar_measurements_buffer_free(cluster->buffer);
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

/**
 * @file radar_processer.c
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief LFMCW雷达信号处理，当前版本RDM输入为 complex_int16_t 类型
 *
 * @note  应用算法步骤如下
 *          1. radardsp_init()初始化参数并分配内存。
 *          2. 调用 radardsp_register_hook_<type>
 * 注册钩子函数，钩子函数会在信号处理个各个阶段依次调用，用于获取雷达算法的处理结果。
 *          3. 然后每收到一帧数据就调用radardsp_input_new_frame()输入数据。
 *
 * @version 0.1
 * @date 2024-11-20
 *
 * @copyright Copyright (c) 2024
 *
 */


#include <radar/app/processer_fixed.h>

#include <radar/sp/fixed_point/radar_mag.h>
#include <radar/sp/fixed_point/radar_cfar.h>
#include <radar/sp/fixed_point/radar_dis_velo.h>
#include <radar/sp/fixed_point/radar_doa.h>
#include <radar/sp/fixed_point/radar_cluster.h>

#include <radar/common/radar_log.h>
#include <radar/common/mm.h>

#include <string.h>
#include <math.h>


/**
 * @brief CFAR中的每一个速度为0的点，都一次查询微动信息，不符合要求的点删除
 *
 * @param radar
 */
static void check_and_delete_static_point(radar_handle_t *radar) { }


/**
 * @brief 聚类，输入当前帧的量测值和历史数据，输出当前帧的聚类结果
 *        该函数会将当前帧的量测值和历史数据组合成一个大矩阵，然后对
 *        该矩阵进行DBSCAN聚类，聚类的结果存储到radar->cluster_meas中
 *
 * @param radar   雷达句柄
 * @param newFrame 当前帧的量测值
 */
static int point_clouds_clustering(radar_handle_t *radar, measurements_t *newFrame)
{
    int status;
    radar_cluster_t *cluster = &radar->cluster;
    measurements_buffer_t *buf = cluster->buffer;
    size_t num_cluster;

    /* 如果缓冲区满了，删除最早的一帧 */
    if (radar_measurements_buffer_framenum(buf) >= buf->gp_size - 1) {
        radar_measurements_buffer_pop(buf);
    }

    /* 将当前帧的量测值放入缓冲区 */
    radar_measurements_buffer_push(buf, newFrame);

    /* 将缓冲区中的所有量测值复制到multi_frame_meas中 */
    status = radar_measurements_buffer_copyout(cluster->multi_frame_meas, buf);
    if (status) {
        RD_WARN("Warning! radar_init_param_t::numMaxMeas too small\n");
    }

    /* DBSCAN */
    status = radar_cluster_dbscan(           //
        cluster->dbscan_handle,              // 预分配的DBSCAN句柄
        cluster->multi_frame_meas_labels,    // 聚类后的标签
        &num_cluster,                        // 标签数量
        cluster->multi_frame_meas,           // 待聚类的量测值
        radar->config.dbscan_cfg.wr,         // 距离权重
        radar->config.dbscan_cfg.wv,         // 速度权重
        radar->config.dbscan_cfg.eps,        // 广义距离阈值
        radar->config.dbscan_cfg.min_samples // 每个簇的最小点数
    );

    if (status) {
        if (status == -RADAR_ECAPACITY) {
            /* DBSCAN预分配的内存不足，建议修改初始化参数(偶尔的情况可以忽略) */
            RD_WARN("[DBSCAN] Warning! radar_handle_t::cluster::dbscan_handle is out of memory.\n");
        } else if (status == -RADAR_ENOMEM) {
            /* 堆内存不足 */
            RD_ERROR("[DBSCAN] Error! Hea is out of memory.\n");
        } else {
            RD_ERROR("[DBSCAN] Error! Unknown error %d.\n", status);
            RADAR_ASSERT(0);
        }
    }

    /* 点云融合 */
    radar_cluster_fusion(cluster->cluster_meas, num_cluster, cluster->multi_frame_meas_labels,
        cluster->multi_frame_meas);
    return 0;
}


/**
 * @brief 雷达句柄初始化
 *
 * @param radar
 * @param param     雷达参数，描述雷达的基本性质，不影响算法的性能
 * @param config    雷达配置，雷达算法各个步骤的配置
 * @return int    0: 成功 -1: 失败
 */
int radardsp_init(radar_handle_t *radar, radar_init_param_t *param, radar_config_t *config)
{
    int status = 0;
    radar->cntFrame = 0;

    /* 设置参数 */
    radar->param.wavelength = param->wavelength;
    radar->param.bandwidth = param->bandwidth;
    radar->param.timeChirpPeriod = param->timeChirpPeriod;
    radar->param.timeFramePeriod = param->timeFramePeriod;
    radar->param.numChannel = param->numChannel;
    radar->param.numRangeBin = param->numRangeBin;
    radar->param.numChirp = param->numChirp;

    radar->param.timeFrameDuration = radar->param.numChirp * radar->param.timeChirpPeriod;
    radar->param.resRange = (LIGHT_SPEED / 2) / radar->param.bandwidth * 1000;
    radar->param.resVelocity =
        radar->param.wavelength / (2 * radar->param.timeFrameDuration) * 1000;
    radar->param.lambda_over_d_q15 =
        radar->param.wavelength / param->rx_antenna_spacing * ((int32_t)1 << 15);

    /* 设置配置 */
    memcpy(&radar->config, config, sizeof(radar_config_t));

    /* 初始化基本数据 */
    status = radar_basic_data_init(&radar->basic, &radar->param);
    if (status != 0) {
        status = 1;
        goto RADARDSP_INIT_FAILED1;
    }

    /* 初始化微动检测 */
    status = radar_micromotion_handle_init(&radar->micromotion, radar->param.numRangeBin,
        (size_t)(4.0 / radar->param.timeFramePeriod));
    if (status != 0) {
        status = 2;
        goto RADARDSP_INIT_FAILED2;
    }

    /* 初始化CFAR */
    radar->cfar = cfar2d_result_alloc(param->numMaxCfarPoints);
    if (radar->cfar == NULL) {
        status = 3;
        goto RADARDSP_INIT_FAILED3;
    }

    /* 初始化量测值（一帧） */
    radar->meas = radar_measurements_alloc(param->numMaxCfarPoints);
    if (radar->meas == NULL) {
        status = 3;
        goto RADARDSP_INIT_FAILED3;
    }

    /* 初始化聚类 */
    status = radar_cluster_init(
        &radar->cluster, param->numMaxCachedFrame, param->numMaxMeas, param->numMaxCluster);
    if (status != 0) {
        status = 4;
        goto RADARDSP_INIT_FAILED4;
    }

    /* 初始化钩子函数 */
    status = radar_hook_init(&radar->hook);
    if (status != 0) {
        status = 5;
        goto RADARDSP_INIT_FAILED5;
    }

    /* 初始化跟踪 */
    radar->tracker = tracker_new(&radar->config.tracker_cfg);
    radar->tracked_targets = tracked_targets_list_new();
    radar->unconfirmed_targets = tracked_targets_list_new();


    return 0;

RADARDSP_INIT_FAILED5:
    radar_cluster_deinit(&radar->cluster);
RADARDSP_INIT_FAILED4:
    cfar2d_result_free(radar->cfar);
RADARDSP_INIT_FAILED3:
    radar_micromotion_handle_deinit(&radar->micromotion);
RADARDSP_INIT_FAILED2:
    radar_basic_data_deinit(&radar->basic);
RADARDSP_INIT_FAILED1:

    return status;
}


void radardsp_register_hook_cfar_raw(radar_handle_t *radar, void (*func)(const cfar2d_result_t *))
{
    radar->hook.hook_cfar_raw = func;
}


void radardsp_register_hook_cfar_filtered(
    radar_handle_t *radar, void (*func)(const cfar2d_result_t *))
{
    radar->hook.hook_cfar_filtered = func;
}


void radardsp_register_hook_point_clouds(
    radar_handle_t *radar, void (*func)(const measurements_t *))
{
    radar->hook.hook_point_clouds = func;
}

void radardsp_register_hook_point_clouds_filtered(
    radar_handle_t *radar, void (*func)(const measurements_t *))
{
    radar->hook.hook_point_clouds_filtered = func;
}


void radardsp_register_hook_clusters(radar_handle_t *radar, void (*func)(const measurements_t *))
{
    radar->hook.hook_clusters = func;
}


static int cb_set_track_meas(rd_float_t *meas, size_t capacity, void *args)
{
    measurements_t *src = (measurements_t *)args;
    rd_float_t *dst = meas;
    size_t n = src->num;
    RADAR_ASSERT(capacity >= n);
    for (size_t i = 0; i < n; i++) {
        *dst++ = (rd_float_t)(src->data[i].azimuth) / (rd_float_t)(8192.0);
        *dst++ = (rd_float_t)(src->data[i].distance) / (rd_float_t)(1000.0);
        *dst++ = (rd_float_t)(src->data[i].velocity) / (rd_float_t)(1000.0);
    }
    return 0;
}

/**
 * @brief 输入一帧RDM
 *
 * @param radar         雷达句柄
 * @param rdms          一帧RDM，三维数组，维度依次为[通道,距离,速度]
 * @param timestamp_ms  时间戳，单位毫秒
 * @return int
 */
int radardsp_input_new_frame(
    radar_handle_t *radar, matrix3d_complex_int16_t *rdms, uint32_t timestamp_ms)
{
    int status;
    measurements_t *meas = radar->meas;

    RADAR_ASSERT(rdms != NULL && rdms->size0 == radar->param.numChannel &&
        rdms->size1 == radar->param.numRangeBin && rdms->size2 == radar->param.numChirp);
    RD_DEBUG("开始处理一帧数据\r\n");

    /*
     * 输入一帧RDM（2D-FFT后的产物）
     * RDM的两个维度是(距离，速度)，速度是没有结果fftshift的，所以说后一半是负速度，前一半是正速度
     */
    radar->basic.rdms = rdms;


#if ENABLE_STATIC_CLUTTER_FILTERING == ON
    /* 1. 更新静态杂波，并减去静态杂波 */

#endif /* ENABLE_STATIC_CLUTTER_FILTERING */


#if AMPLITUDE_SPECTRUM_CALCULATION_METHOD == AMP_SPEC_CLAC_METHOD_INSIDE

    /* 2. 计算幅度谱 */
    radar_clac_magSpec2D(        //
        radar->basic.magSpec2D,  // 幅度谱
        radar->basic.rdms,       // RDM
        radar->param.numChannel, // 需要累加的通道数
        0                        // 起始RDM编号
    );

#endif /* AMPLITUDE_SPECTRUM_CALCULATION_METHOD */

    /* 3. 维护微动信息 */
    radar_micromotion_add_frame(&radar->micromotion, radar->basic.rdms);

    /* 4. CFAR搜索点，最终输出的检测结果包含点的 */
    radar_cfar2d_goca(radar->cfar, radar->basic.magSpec2D, &radar->config.cfarCfg);

    if (radar->hook.hook_cfar_raw != NULL) {
        radar->hook.hook_cfar_raw(radar->cfar);
    }

    /* 5. 点云凝聚： 删除CFAR结果中一些幅度较小的点
     *
     *  一个目标的信号往往会分散到多个单元中，部分单元中的能量较小，导致测角精度低，
     *  进而导致点云聚类的时候不能很好的将这些点分到一个簇中，因此要提前把这些点删除掉
     */
    radar_cfar_result_filtering(radar->cfar, &radar->config.cfar_filter_cfg);


    /* 6. CFAR结果中的0速度点查询微动信息，删除不符合要求的点 */
    check_and_delete_static_point(radar);

    if (radar->hook.hook_cfar_filtered != NULL) {
        radar->hook.hook_cfar_filtered(radar->cfar);
    }

    /* 7. 计算角度，删除一部分可能导致角度模糊的点 */
    meas->num = 0;
    radar_dual_channel_clac_angle(                  //
        meas,                                       // 量测值数组
        radar->cfar,                                // CFAR结果
        radar->basic.rdms,                          // RDM
        radar->param.lambda_over_d_q15,             // 波长/天线间距
        radar->config.channel_phase_diff_threshold, // 通道间相位差阈值
        radar->config.channel_mag_diff_threshold    // 通道间幅度差阈值
    );


    /* 8. 计算速度和距离 */
    radar_clac_dis_and_velo(     //
        meas,                    // 量测值数组
        radar->cfar,             // CFAR结果
        radar->basic.magSpec2D,  // 幅度谱
        radar->param.resRange,   // 距离分辨率
        radar->param.resVelocity // 速度分辨率
    );

    RADAR_ASSERT(radar->cfar->numPoint == meas->num);

    if (radar->hook.hook_point_clouds != NULL) {
        radar->hook.hook_point_clouds(meas);
    }


    /* 删除被遮挡的点 */
    radar_measure_delete_obscured(meas, radar->config.occlusion_radius);

    if (radar->hook.hook_point_clouds_filtered != NULL) {
        radar->hook.hook_point_clouds_filtered(meas);
    }

    /* 9. 二维平面聚类(DBSCAN)
     *
     *  累计多帧数据再做聚类。 因为杂波点往往不会连续多次在小范围内出现,
     *  合理设置聚类的簇最小点数，可以避免杂波点聚类成一个簇。另外当目标
     *  偶尔丢失时，累计多帧数据再做聚类，可以避免丢失
     */
    status = point_clouds_clustering(radar, meas);
    if (status) {
        RADAR_LOG_PRINTF("Warning! point_clouds_clustering failed.\n");
        return -9;
    }

    if (radar->hook.hook_clusters != NULL) {
        radar->hook.hook_clusters(radar->cluster.cluster_meas);
    }

    /* 10. 目标跟踪 */
    RD_DEBUG("运行目标跟踪");
    tracker_run(radar->tracker, radar->tracked_targets, radar->unconfirmed_targets,
        radar->cluster.cluster_meas->num, timestamp_ms, cb_set_track_meas,
        radar->cluster.cluster_meas);

    if (radar->hook.hook_unconfirmed_targets != NULL) {
        radar->hook.hook_unconfirmed_targets(radar->unconfirmed_targets);
    }
    if (radar->hook.hook_tracked_targets != NULL) {
        radar->hook.hook_tracked_targets(radar->tracked_targets);
    }

    radar->cntFrame++;
    return 0;
}

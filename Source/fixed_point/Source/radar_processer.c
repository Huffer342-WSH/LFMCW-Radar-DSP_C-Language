/**
 * @file radar_processer.c
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief LFMCW雷达信号处理，当前版本RDM输入为 complex_int16_t 类型
 *
 * @note  应用算法步骤如下
 *          1. radardsp_init()初始化参数并分配内存。
 *          2. 调用 radardsp_register_hook_<type> 注册钩子函数，钩子函数会在信号处理个各个阶段依次调用，用于获取雷达算法的处理结果。
 *          3. 然后每收到一帧数据就调用radardsp_input_new_frame()输入数据。
 *
 * @version 0.1
 * @date 2024-11-20
 *
 * @copyright Copyright (c) 2024
 *
 */


#include "radar_processer.h"

#include "radar_mag.h"
#include "radar_cfar.h"
#include "radar_dis_velo.h"
#include "radar_doa.h"
#include "radar_cluster.h"

#include "radar_assert.h"
#include "radar_error.h"


#include <stdlib.h>
#include <string.h>
#include <math.h>


static void check_and_delete_static_point(radar_handle_t *radar);
static void point_clouds_clustering(radar_handle_t *radar, measurements_t *newFrame);


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
    radar->param.timeChrip = param->timeChrip;
    radar->param.timeChripGap = param->timeChripGap;
    radar->param.timeFrameGap = param->timeFrameGap;
    radar->param.numChannel = param->numChannel;
    radar->param.numRangeBin = param->numRangeBin;
    radar->param.numChrip = param->numChrip;

    radar->param.timeChripFull = radar->param.timeChrip + radar->param.timeChripGap;
    radar->param.timeFrameVaild = radar->param.numChrip * radar->param.timeChripFull;
    radar->param.timeFrameTotal = radar->param.timeFrameVaild + radar->param.timeFrameGap;
    radar->param.resRange = 149896229.0 / radar->param.bandwidth * 1000;
    radar->param.resVelocity = radar->param.wavelength / (2 * radar->param.timeFrameVaild) * 1000;
    radar->param.lambda_over_d_q15 = radar->param.wavelength / param->rx_antenna_spacing * ((int32_t)1 << 15);

    /* 设置配置 */
    memcpy(&radar->config, config, sizeof(radar_config_t));

    /* 初始化基本数据 */
    status = radar_basic_data_init(&radar->basic, &radar->param);
    if (status != 0) {
        status = 1;
        goto RADARDSP_INIT_FAILED1;
    }

    /* 初始化微动检测 */
    status = radar_micromotion_handle_init(&radar->micromotion, radar->param.numRangeBin, (size_t)(4.0 / radar->param.timeFrameTotal));
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


    /* 初始化聚类 */
    status = radar_cluster_init(&radar->cluster, param->numMaxCachedFrame, param->numInitialMultiMeas, param->numInitialCluster);
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


void radardsp_register_hook_cfar_filtered(radar_handle_t *radar, void (*func)(const cfar2d_result_t *))
{
    radar->hook.hook_cfar_filtered = func;
}


void radardsp_register_hook_point_clouds(radar_handle_t *radar, void (*func)(const measurements_t *))
{
    radar->hook.hook_point_clouds = func;
}

void radardsp_register_hook_point_clouds_filtered(radar_handle_t *radar, void (*func)(const measurements_t *))
{
    radar->hook.hook_point_clouds_filtered = func;
}


void radardsp_register_hook_clusters(radar_handle_t *radar, void (*func)(const measurements_t *))
{
    radar->hook.hook_clusters = func;
}

/**
 * @brief 输入一帧RDM
 *
 * @param radar 雷达句柄
 * @param data 一帧RDM，三维数组，维度依次为[通道,距离,速度]
 * @param size data的大小，单位int16。 冗余参数，增加代码可靠性
 * @return int
 */
int radardsp_input_new_frame(radar_handle_t *radar, matrix3d_complex_int16_t *rdms)
{
    RADAR_ASSERT(rdms != NULL && rdms->size0 == radar->param.numChannel && rdms->size1 == radar->param.numRangeBin && rdms->size2 == radar->param.numChrip);

    /*
    输入一帧RDM（2D-FFT后的产物）
    RDM的两个维度是(距离，速度)，速度是没有结果fftshift的，所以说后一半是负速度，前一半是正速度
    */
    radar->basic.rdms = rdms;


#if ENABLE_STATIC_CLUTTER_FILTERING == ON
    /* 1. 更新静态杂波，并减去静态杂波 */

#endif /* ENABLE_STATIC_CLUTTER_FILTERING */


#if AMPLITUDE_SPECTRUM_CALCULATION_METHOD == AMP_SPEC_CLAC_METHOD_INSIDE

    /* 2. 计算幅度谱 */
    radar_clac_magSpec2D(radar->basic.magSpec2D,  // 幅度谱
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

    /* 5. 点云凝聚： 删除CFAR结果中一些幅度较小的点 */
    /* 一个目标的信号往往会分散到多个单元中，部分单元中的能量较小，导致测角精度低，\
        进而导致点云聚类的时候不能很好的将这些点分到一个簇中，因此要提前把这些点删除掉 */
    radar_cfar_result_filtering(radar->cfar, &radar->config.cfar_filter_cfg);


    /* 6. CFAR结果中的0速度点查询微动信息，删除不符合要求的点 */
    check_and_delete_static_point(radar);

    if (radar->hook.hook_cfar_filtered != NULL) {
        radar->hook.hook_cfar_filtered(radar->cfar);
    }


    measurements_t *one_frame_meas = radar_measurements_alloc(radar->cfar->numPoint);
    if (one_frame_meas == NULL) {
        RADAR_ERROR("radar_measurements_alloc failed", RADAR_ENOMEM);
        return -1;
    }
    printf("radar_measurements_alloc ok\n");

    /* 7. 计算角度，删除一部分可能导致角度模糊的点 */
    radar_dual_channel_clac_angle(one_frame_meas, radar->cfar, radar->basic.rdms, radar->param.lambda_over_d_q15, radar->config.channel_phase_diff_threshold,
                                  radar->config.channel_mag_diff_threshold);

    RADAR_ASSERT(radar->cfar->numPoint == one_frame_meas->num);

    /* 8. 计算速度和距离 */
    radar_clac_dis_and_velo(one_frame_meas, radar->cfar, radar->basic.magSpec2D, radar->param.resRange, radar->param.resVelocity);


    if (radar->hook.hook_point_clouds != NULL) {
        radar->hook.hook_point_clouds(one_frame_meas);
    }


    /* 删除被遮挡的点 */
    radar_measure_delete_obscured(one_frame_meas, radar->config.occlusion_radius);
    if (radar->hook.hook_point_clouds_filtered != NULL) {
        radar->hook.hook_point_clouds_filtered(one_frame_meas);
    }

    /* 9. 二维平面聚类(DBSCAN) */
    /* 累计多帧数据再做聚类。 因为杂波点往往不会连续多次在小范围内出现，而目标信号可以。
      合理设置聚类的簇最小点数，可以避免杂波点聚类成一个簇
      另外当目标偶尔丢失时，累计多帧数据再做聚类，可以避免丢失
    */
    point_clouds_clustering(radar, one_frame_meas);
    radar_measurements_free(one_frame_meas);
    if (radar->hook.hook_clusters != NULL) {
        radar->hook.hook_clusters(radar->cluster.cluster_meas);
    }


#if 0
    /*=========================================================================
        到此为止，信号处理已经结束，得到的是新一帧的目标检测结果
        接下来主要是完成目标跟踪，也就是把不同帧检测出来的结果前后关联起来，得到一个个目标的运动轨迹
    =========================================================================*/

    /* 6. 目标更新：包含已有目标的关联或者删除，以及添加新的目标
        关联： 使用目标在上一帧的位置，或者是预测出来的这一帧可能的位置 和
       这一帧检测出来的目标匹配，将匹配上的点加入到已有的轨迹中 推测：
       一个正在跟踪的目标没有成功匹配到新一帧目标时，不立马删除，开始推测（直接采用上一帧的预测值）。
        删除： 加入一个目标长时间没有成功匹配，或者关联后目标离开检测范围，就将这个目标删除。
        添加： 新出现的目标没有被关联过，就添加进来

        当使用卡尔曼滤波时，新添加的目标作为新的量测值用于纠正预测值，得到滤波后的值

        考虑使用打分机制维护目标的删除。
        新目标需要有一个起始阶段，因为收到噪声干扰雷达会检测到一些假目标，这些假目标需要在没关联上时快速删除
        新添加进来的目标给一个初始分数，如果成功关联上就快速加分，否则就扣分，这样可以把假目标快速删掉；
        同时分数要设置上限，防止分数太高导致删不掉；
        目标处于静止状态时加分要减少
    */

    /* 7. 目标预测（可选）：根据已有的轨迹，预测下一帧的目标位置。
        一方面预测一帧的时间，可以在下一帧关联时更容易匹配到正确的目标
        另一方面，目标检测在低信噪比时会时隐时现，常常需要预测几帧数据。

        当使用卡尔曼滤波时，这里就用与实现卡尔曼预测
   */

    /*
        注意，为了避免非线性关系，坐标采用雷达信号处理出来的极坐标，而不是使用直角坐标系。
        因为在卡尔曼滤波中，状态转移方程是用一个矩阵相乘来表示的，加入观测值是直角，但是量测值是极坐标，就会导致状态转移方程是非线性的，需要使用扩展卡尔曼。
        （量测值：测出来的原始数据。  观测值：滤波时维护的数据）

        假如只使用直角坐标，相当于丢失了观测噪声在极坐标中的信息，比如测角精度固定，那越近直角坐标就越准吗。本来观测噪声是在极坐标下的，在经过观测矩阵后就可以把这个性质表达到直角坐标中。如果直接使用直角坐标系，就体现不出这个性质。

    */

#endif
    radar->cntFrame++;
    return 0;
}

/**
 * @brief CFAR中的每一个速度为0的点，都一次查询微动信息，不符合要求的点删除
 *
 * @param radar
 */
static void check_and_delete_static_point(radar_handle_t *radar)
{
}


/**
 * @brief 聚类，输入当前帧的量测值和历史数据，输出当前帧的聚类结果
 *        该函数会将当前帧的量测值和历史数据组合成一个大矩阵，然后对
 *        该矩阵进行DBSCAN聚类，聚类的结果存储到radar->cluster_meas中
 *
 * @param radar   雷达句柄
 * @param newFrame 当前帧的量测值
 */
static void point_clouds_clustering(radar_handle_t *radar, measurements_t *newFrame)
{
    radar_cluster_t *cluster = &radar->cluster;
    measurements_list_t *list = cluster->list;

    /* 新一帧的量测值添加到队列 */
    radar_measurements_list_push(list, newFrame);

    /* 将多帧的量测值复制到一个measurements_t中 */
    size_t num_meas = radar_measurements_list_get_meas_num(list);
    if (cluster->multi_frame_meas->capacity < num_meas) {
        radar_measurements_free(cluster->multi_frame_meas);
        free(cluster->multi_frame_meas_labels);
        cluster->multi_frame_meas = radar_measurements_alloc(num_meas);
        cluster->multi_frame_meas_labels = malloc(sizeof(size_t) * num_meas);
    }
    radar_measurements_list_copyout(cluster->multi_frame_meas, list);

    /* DBSCAN */
    int num_cluster = radar_cluster_dbscan(cluster->multi_frame_meas_labels,    //
                                           cluster->multi_frame_meas,           //
                                           radar->config.dbscan_cfg.wr,         //
                                           radar->config.dbscan_cfg.wv,         //
                                           radar->config.dbscan_cfg.eps,        //
                                           radar->config.dbscan_cfg.min_samples //

    );

    /* 点云融合 */
    if (cluster->cluster_meas->capacity < num_cluster) {
        radar_measurements_free(cluster->cluster_meas);
        cluster->cluster_meas = radar_measurements_alloc(num_cluster);
    }
    radar_cluster_fusion(cluster->cluster_meas, num_cluster, cluster->multi_frame_meas_labels, cluster->multi_frame_meas);
}

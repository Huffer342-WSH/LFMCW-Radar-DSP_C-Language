/**
 * @file radar_clutter_filter.c
 * @brief 提供杂波滤除相关功能
 * @version 0.4
 * @date 2025-09-12
 */

#include <radar/sp/fixed_point/radar_clutter_filter.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief new 一个静态杂波滤波器
 *
 * @param cfg
 * @return radar_clutter_filter_t*
 */
radar_clutter_filter_t *radar_static_clutter_filter_new(const radar_clutter_filter_cfg_t *cfg)
{
    radar_clutter_filter_t *filter;
    size_t total = cfg->n_channel * cfg->n_rb;
    int64_t gain;

    filter = (radar_clutter_filter_t *)malloc(sizeof(radar_clutter_filter_t));
    if (filter == NULL) {
        goto errout;
    }

    filter->static_clutter = (static_clutter_t *)calloc(total, sizeof(static_clutter_t));
    if (!filter->static_clutter) {
        goto errout;
    }

    for (size_t k = 0; k < total; k++) {
        static_clutter_t *sc = &filter->static_clutter[k];
        sc->fifo = (int16_t *)calloc(cfg->fifo_len, sizeof(int16_t));
        if (sc->fifo == NULL) {
            goto errout;
        }
    }

    filter->n_channel = cfg->n_channel;
    filter->n_rb = cfg->n_rb;
    filter->fifo_len = cfg->fifo_len;
    filter->weight_max = cfg->weight_max;
    filter->weight_min = cfg->weight_min;
    filter->update_interval = cfg->update_interval;

    gain = ((int64_t)(cfg->weight_max - cfg->weight_min) << 20) / (cfg->phase_max - cfg->phase_min);
    filter->weight_gain = gain > INT32_MAX ? INT32_MAX : gain;

    filter->fifo_in = 0;
    filter->fifo_count = 0;
    filter->frame_count = 0;

    return filter;
errout:
    if (filter) {
        if (filter->static_clutter) {
            for (size_t i = 0; filter->static_clutter[i].fifo; i++) {
                free(filter->static_clutter[i].fifo);
            }
            free(filter->static_clutter);
        }
        free(filter);
    }

    return NULL;
}

/**
 * @brief delete 一个静态杂波滤波器
 *
 * @param filter
 */
void radar_static_clutter_filter_delete(radar_clutter_filter_t *filter)
{
    size_t total = filter->n_channel * filter->n_rb;
    for (size_t k = 0; k < total; k++) {
        free(filter->static_clutter[k].fifo);
    }
    free(filter->static_clutter);
    filter->static_clutter = NULL;
}

/**
 * @brief 静态杂波滤波器。起到快速抑制静态杂波，同时保留微动目标信号的效果
 *
 * @param filter 滤波器结构体
 * @param[inout] rdm RDM矩阵，维度为[通道、距离、速度]
 * @return int
 */
int radar_static_clutter_filter(radar_clutter_filter_t *filter, matrix3d_complex_int16_t *rdm)
{
    RADAR_ASSERT_EQ(filter->n_channel, rdm->size0);
    RADAR_ASSERT_EQ(filter->n_rb, rdm->size1);

    size_t n_channel = filter->n_channel;
    size_t n_rb = filter->n_rb;
    size_t tda = rdm->tda2 * 2;
    int16_t *src = rdm->data;
    uint16_t fifo_in;
    int fifo_full;

    fifo_in = filter->fifo_in;
    fifo_full = filter->fifo_count >= filter->fifo_len;

    /* 遍历所有单元，先累积本帧的均值 */
    for (size_t i = 0; i < n_channel; i++) {
        for (size_t j = 0; j < n_rb; j++) {
            static_clutter_t *sc = &filter->static_clutter[i * n_rb + j];

            int16_t real = *src;
            int16_t imag = *(src + 1);
            int16_t curr_phase;
            int16_t phase_diff;

            /* 计算相位差 */
            curr_phase = radar_atan2_q15(imag, real);
            phase_diff = (int16_t)abs(unwrap_phase_diff_q13(curr_phase, sc->prev_phase));
            sc->prev_phase = curr_phase;

            /* 相位差压入FIFO吗，计算累积相位差delta_phase */
            if (fifo_full) {
                sc->delta_phase -= sc->fifo[fifo_in];
            }
            sc->fifo[fifo_in] = phase_diff;
            sc->delta_phase += phase_diff;

            /* 累积用于 interval 均值 */
            sc->accum_real += real;
            sc->accum_imag += imag;

            /* 去均值（使用上一次的mean） */
            *src = real - sc->mean_real;
            *(src + 1) = imag - sc->mean_imag;

            src += tda;
        }
    }

    /* 更新全局FIFO索引 */
    if (++fifo_in >= filter->fifo_len) {
        filter->fifo_in = fifo_in - filter->fifo_len;
    }
    if (!fifo_full) {
        filter->fifo_count++;
    }

    /* 到达更新间隔时，统一更新均值 */
    if (++filter->frame_count >= filter->update_interval) {
        for (size_t k = 0; k < n_channel * n_rb; k++) {
            static_clutter_t *sc = &filter->static_clutter[k];
            int32_t cnt = filter->update_interval;
            int64_t alpha; // Q.15
            int64_t delta_phase = sc->delta_phase;

            /* 计算权重alpha
             *
             * 根据一段时间的累积相位差delta_phase，计算权重alpha。
             * 基本符合delta_phase越大，alpha越小
             */
            if (delta_phase < filter->phase_min) {
                delta_phase = 0;
            } else {
                delta_phase -= filter->phase_min;
            }
            alpha = filter->weight_max;
            alpha -= ((delta_phase * filter->weight_gain) >> 20);
            if (alpha < filter->weight_min) {
                alpha = filter->weight_min;
            }

            /* 更新总均值
             *
             * 新均值 = 旧均值 + (新值 - 旧均值) * alpha
             * mean = mean + (accum/cnt - mean) * alpha
             *      = mean + (accum - mean * cnt) * alpha / cnt
             *
             * 为了保证alpha定点数的精度，先左移5位，结果再右移5位
             * alpha本身是Q.15格式，因此计算结果需要右移15位，共右移(15+5)位
             */
            alpha = (alpha << 5) / cnt;
            sc->mean_real += ((sc->accum_real - sc->mean_real * cnt) * alpha) >> (15 + 5);
            sc->mean_imag += ((sc->accum_imag - sc->mean_imag * cnt) * alpha) >> (15 + 5);

            sc->accum_real = 0;
            sc->accum_imag = 0;
        }

        filter->frame_count = 0; // 重置
    }

    return 0;
}

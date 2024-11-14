#include "radar_cluster.h"

#include "radar_error.h"

#include <stdlib.h>
#include <stdio.h>


/**
 * @brief DBSCAN核心
 *
 * @param[out] labels 标签，-1代表杂点
 * @param n 样本数量
 * @param eps 领域半径，Epsilon
 * @param min_samples 核心的最小样本数
 * @param get_neighbors 回调函数，get_neighbors(&count, i, eps, param) 返回保存邻居的序号的数组，count 为邻居个数
 * @param param 回调函数的额外参数
 */
void dbscan_core(size_t *labels, size_t n, int32_t eps, size_t min_samples, cb_get_neighbors_t get_neighbors, void *param)
{
    size_t label_num = 0;
    for (size_t i = 0; i < n; i++) {
        labels[i] = -1;
    }

    size_t *stack = (size_t *)malloc(n * sizeof(size_t));
    size_t stack_top = 0;

    size_t *neighbors = NULL;
    size_t count = 0;

    for (size_t i = 0; i < n; i++) {
        if (labels[i] != -1) {
            continue;
        }
        neighbors = get_neighbors(&count, i, eps, param);
        if (count < min_samples)
            continue;
        size_t j = i;
        while (1) {
            if (labels[j] == -1) {
                labels[j] = label_num;
                if (j != i) {
                    neighbors = get_neighbors(&count, i, eps, param);
                }
                if (count >= min_samples) {
                    for (size_t k = 0; k < count; k++) {
                        if (labels[neighbors[k]] == -1) {
                            stack[stack_top++] = neighbors[k];
                        }
                    }
                }
            }
            if (stack_top == 0) {
                break;
            }
            j = stack[--stack_top];
        }
        label_num++;
    }
    free(stack);
}

/**
 * @brief  计算两侧值之间的距离
 *
 * @param meas 两侧值列表
 * @param wr Q15.16 平面距离的权重
 * @param wv Q15.16 速度的权重
 * @return int32_t* 保存量测点之间距离的数组，长度为N*(N-1)/2，需要手动释放。
 *         排列顺序为
 *          0:(1,0)
 *          1:(2,0) 2:(2,1)
 *          3:(3,0) 4:(3,1) 5:(3,2)
 *          ...
 *          即(i,j)对应序号为 i*(i-1)/2 + j
 */
int32_t *radar_cluster_calc_distance(radar_measurement_list_fixed_t *meas, int32_t wr, int32_t wv)
{
    // d = sqrt( (c*wr)^2 + (v*wv)^2 )/(wr + wv)
    const size_t n = meas->num;
    int32_t *D = (int32_t *)malloc(n * (n - 1) / 2 * sizeof(int32_t));
    for (size_t i = 1; i < n; i++) {
        const size_t offset = i * (i - 1) / 2;
        for (size_t j = 0; j < i; j++) {
            int32_t v = abs_diff(meas->meas[i].sin_azimuth, meas->meas[j].sin_azimuth);

            // c^ = a^2 + b^2 - 2 * a * b * cos(theta)
            int32_t a = meas->meas[i].distance;
            int32_t b = meas->meas[j].distance;
            int16_t theta_q13 = abs_diff(meas->meas[i].azimuth, meas->meas[j].azimuth);
            int32_t norm_theta_q31 = (int32_t)((int64_t)theta_q13 * ((int64_t)1 << 30) / (PI_Q13));
            int64_t c2;
            c2 = (int64_t)a * radar_cos_q31(norm_theta_q31) >> 31;
            c2 *= (int64_t)b * -2;
            c2 += (int64_t)a * a + b * b;

            int64_t sum;

            int64_t wr2_q31 = ((int64_t)wr * wr) >> 1;
            int64_t wv2_q31 = ((int64_t)wv * wv) >> 1;
            sum = (wr2_q31 * c2 + wv2_q31 * v * v) >> 31;
            if (sum < 0 || sum > INT32_MAX) {
                RADAR_ERROR("radar_cluster_calc_distance overflow", RADAR_EOVRFLW);
            }
            D[offset + j] = radar_sqrt_q31((int32_t)sum * 2) >> 16;
        }
    }
    return D;
}

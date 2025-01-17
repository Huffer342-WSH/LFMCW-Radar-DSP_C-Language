/**
 * @file radar_dis_velo.c
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 根据CFAR结果计算距离与速度
 * @version 0.1
 * @date 2024-11-20
 *
 * @copyright Copyright (c) 2024
 *
 */


#include "radar_dis_velo.h"

#include "radar_assert.h"
#include "radar_error.h"


/**
 * @brief 计算速度与距离
 *
 * @details 根据目标点与相邻点的幅度，加权计算距离，实现超分辨率测距
 *
 * @details idxR = a + b/(r+1)
 *          a = idxR,
 *          b = mag(a+1) > mag(a-1) ? 1:-1
 *          r = mag(a) / mag(a+b)
 *
 * @param meas 量测值列表
 * @param cfar cfar结果
 * @param mag 幅度谱
 * @param resRange 距离分辨率
 * @param resVel 速度分辨率
 * @return int
 */
int radar_clac_dis_and_velo(measurements_t *meas, const cfar2d_result_t *cfar, const matrix2d_int32_t *mag, int32_t resRange, int32_t resVel)
{
#define MAG(x, y) mag->data[(x) * mag->tda1 + (y)]
    RADAR_ASSERT(meas->capacity >= cfar->numPoint);
    for (size_t i = 0; i < cfar->numPoint; i++) {
        const cfar2d_point_t *point = &cfar->point[i];
        measurement_t *m = &meas->data[i];

        int64_t idxR_q16;
        int32_t idxV = point->idx1;

        int32_t a;
        int64_t b_q32, r_q16;
        int64_t denominator;

        a = point->idx0;
        if (a == 0) {
            a = 1;
            b_q32 = -((int64_t)1 << 32);
        } else if (a + 1 == mag->size0 || MAG(a - 1, idxV) > MAG(a + 1, idxV)) {
            b_q32 = -((int64_t)1 << 32);
            denominator = MAG(a - 1, idxV);
        } else if (MAG(a - 1, idxV) < MAG(a + 1, idxV)) {
            b_q32 = (int64_t)1 << 32;
            denominator = MAG(a + 1, idxV);
        } else {
            b_q32 = 0;
            denominator = MAG(a, idxV);
        }
        if (denominator == 0) {
            idxR_q16 = (int64_t)a << 16;
        } else {
            r_q16 = ((int64_t)MAG(a, idxV) << 16) / denominator;
            idxR_q16 = ((int64_t)a << 16) + b_q32 / (r_q16 + ((int64_t)1 << 16));
        }

        int64_t distance = (idxR_q16 * resRange) >> 16;
        int64_t velocity = (int64_t)resVel * ((idxV >= (int32_t)mag->size1 / 2) ? (idxV - (int32_t)mag->size1) : (idxV));
        if (distance > INT32_MAX) {
            distance = INT32_MAX;
            RADAR_ERROR("An overflow occurs when calculating distances using a fixed number of points", RADAR_EOVRFLW);
        }
        if (velocity < -(int64_t)INT32_MAX || velocity > (int64_t)INT32_MAX) {
            velocity = INT32_MAX;
            RADAR_ERROR("An overflow occurs when the speed is calculated using a fixed number of points", RADAR_EOVRFLW);
        }

        /* 添加到量测值列表 */
        m->amp = point->amp;
        m->snr = point->snr;
        m->distance = distance;
        m->velocity = velocity;
    }
    meas->num = cfar->numPoint;
    return 0;
#undef MAG
}

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

#include <radar/sp/fixed_point/radar_dis_velo.h>

#include <radar/common/radar_log.h>

/**
 * @brief 坐标估计
 * @details 通过中心点的幅度谱值与相邻点的幅度谱值，估计出更精确的坐标
 *          拟合方程由单一频率成分FFT后的幅度谱特性推导而来，上述情况下任意两点就可以计算出频率，
 *          此处应选择幅度较高的点减少噪声的干扰。
 *
 *          为了提升稳定性和对多目标的抗干扰性，采用两种选点方式加权，选点方式如下：
 *          1. 选择最大点max和次大点mid，较为稳定
 *          2. 选择最大点max和次次大点min，抗干扰能力强
 *          加权方式为 w = 0.5 / (r - 1.5)，其中 r = max/min
 *
 * @param idxR      中心坐标
 * @param y0        Mag[idxR-1]
 * @param y1        Mag[idxR]
 * @param y2        Mag[idxR+1]
 * @return int32_t  q15.16格式的坐标估计值
 */
static inline int32_t idx_estimation(uint16_t idxR, int32_t y0, int32_t y1, int32_t y2)
{
    int32_t idx_q16;
    int32_t flag;
    int32_t type;
    idx_q16 = (int32_t)idxR << 16;

    if (y0 > y1 || y1 < y2) {
        /* 直接返回中心点 */
        return idx_q16;
    }

    int32_t max, mid, min;
    int32_t w_q16; // 权重
    int64_t r_q16;

    max = y1;
    if (y0 > y2) {
        mid = y0;
        min = y2;
        flag = -1;
    } else {
        mid = y2;
        min = y0;
        flag = 1;
    }

    r_q16 = min ? ((int64_t)max << 16) / min : INT64_MAX;

    /*
     *  1. w = 1, r < 2
     *  2. w = 0.5 / (r - 1.5), r >= 2
     */
    if (r_q16 < I32Q16(2)) {
        /* x = x + flag * mid/(max+mid) */
        idx_q16 += flag * ((int64_t)I32Q16(1) * mid / ((int64_t)max + mid));
    } else {
        w_q16 = (int64_t)((int64_t)1 << 31) / (int64_t)(r_q16 - I32Q16(1.5));

        /* x = x + flag * (1-w)min/(max-min) + w * mid/(max+mid) */
        idx_q16 += flag *
            ((int64_t)(I32Q16(1) - w_q16) * min / ((int64_t)max - min) +
                (int64_t)w_q16 * mid / ((int64_t)max + mid));
    }

    return idx_q16;
}

/**
 * @brief 计算速度与距离
 *
 * @details 计算距离时依据目标点与相邻点的幅度，加权计算距离，实现超分辨率测距
 *
 * @param meas       量测值列表
 * @param cfar       cfar结果
 * @param mag        幅度谱
 * @param resRange   距离分辨率
 * @param resVel     速度分辨率
 * @return int
 */
int radar_clac_dis_and_velo(measurements_t* meas, const cfar2d_result_t* cfar,
    const matrix2d_int32_t* mag, int32_t resRange, int32_t resVel)
{
#define MAG(idxR, y) mag->data[(idxR) * mag->tda1 + (y)]
    RADAR_ASSERT(meas->capacity >= cfar->numPoint);
    for (size_t i = 0; i < cfar->numPoint; i++) {
        const cfar2d_point_t* point = &cfar->point[i];
        measurement_t* m = &meas->data[i];
        uint16_t idxV = point->idx1;
        uint16_t idxR = point->idx0;
        int64_t distance;
        int64_t velocity;
        int32_t y0, y1, y2;

        /* 计算距离 */
        y0 = (idxR > 0) ? MAG(idxR - 1, idxV) : 0;
        y1 = MAG(idxR, idxV);
        y2 = (idxR < mag->size0 - 1) ? MAG(idxR + 1, idxV) : 0;

        distance = ((int64_t)idx_estimation(idxR, y0, y1, y2) * resRange) >> 16;

        if (distance > INT32_MAX) {
            distance = INT32_MAX;
            RADAR_ERROR(
                "An overflow occurs when calculating distances using a fixed number of points",
                RADAR_EOVRFLW);
        }

        /* 计算速度 */
        if (idxV >= (int32_t)mag->size1 / 2) {
            velocity = (int64_t)resVel * (idxV - (int32_t)mag->size1);
        } else {
            velocity = (int64_t)resVel * idxV;
        }

        if (velocity < -(int64_t)INT32_MAX || velocity > (int64_t)INT32_MAX) {
            velocity = INT32_MAX;
            RADAR_ERROR(
                "An overflow occurs when the speed is calculated using a fixed number of points",
                RADAR_EOVRFLW);
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

/**
 * @file radar_cfar.c
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 2D-CFAR 程序，输入幅度谱输出点云
 * @version 0.1
 * @date 2024-11-05
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "radar_cfar.h"
#include "radar_types.h"

#include "radar_assert.h"
#include "radar_error.h"

#include <stdlib.h>
#include <stdbool.h>

#include <stc/cbits.h>

static inline int unwrapper_neg(int x, int n);
static inline int wrap_pos(int x, int n);


int cfar2d_result_reset(cfar2d_result_t *cfar)
{
    cfar->numPoint = 0;
    return 0;
}

/**
 * @brief 分配2D-CFAR检测结果并初始化
 *
 * @param[in] n 需要分配的CFAR检测点的数量
 * @return cfar2d_result_t* 一个指向初始化的2D-CFAR结果结构的指针，或者在失败时为NULL
 */
cfar2d_result_t *cfar2d_result_alloc(size_t n)
{
    // 分配结果结构
    cfar2d_result_t *res = (cfar2d_result_t *)malloc(sizeof(cfar2d_result_t));
    if (res == NULL) {
        return NULL;
    }

    // 分配点云数据
    res->point = (cfar2d_point_t *)malloc(n * sizeof(cfar2d_point_t));
    if (res->point == NULL) {
        free(res);
        return NULL;
    }

    // 初始化
    res->capacity = n;
    res->numPoint = 0;
    res->is_point_need_free = 1;
    res->is_struct_need_free = 1;

    return res;
}

/**
 * @brief 从预分配的内存块中分配和初始化2D-CFAR检测结果
 *
 * @param block 预分配的内存块
 * @param blockSize 预分配的内存块的大小
 * @param n 需要分配的CFAR检测点的数量
 * @return cfar2d_result_t* 一个指向初始化的2D-CFAR结果结构的指针，或者在失败时为NULL
 */
cfar2d_result_t *cfar2d_result_calloc_static(void *buffer, size_t size, size_t capacity)
{

    RADAR_ASSERT(buffer != NULL);

    cfar2d_result_t *res;
    cfar2d_point_t *pData;
    size_t data_size = capacity * sizeof(cfar2d_point_t);

    res = (cfar2d_result_t *)ALIGN_ADDRESS(buffer, alignof(cfar2d_result_t));
    pData = (cfar2d_point_t *)ALIGN_ADDRESS((uint8_t *)res + sizeof(cfar2d_result_t), alignof(cfar2d_point_t));

    if ((uintptr_t)pData + data_size > (uintptr_t)buffer + size) {
        return NULL;
    }

    res->point = pData;
    res->capacity = capacity;
    res->numPoint = 0;
    res->is_point_need_free = 0;
    res->is_struct_need_free = 0;

    return res;
}

/**
 * @brief 释放2D-CFAR检测结果
 *
 * @param[in] result 2D-CFAR检测结果
 */
void cfar2d_result_free(cfar2d_result_t *result)
{
    if (result == NULL) {
        RADAR_ERROR("cfar2d_result_free a NULL pointer", RADAR_EFREENULL);
        return;
    }

    // 释放内存
    if (result->is_point_need_free) {
        free(result->point);
    }
    if (result->is_struct_need_free) {
        free(result);
    }
}


int cfar2d_result_add_point(cfar2d_result_t *result, uint16_t idx0, uint16_t idx1, int32_t amp, int32_t snr)
{
    if (result->numPoint >= result->capacity) {
        return -1;
    }
    result->point[result->numPoint].idx0 = idx0;
    result->point[result->numPoint].idx1 = idx1;
    result->point[result->numPoint].amp = amp;
    result->point[result->numPoint].snr = snr;
    result->numPoint++;
    return 0;
}
/**
 * @brief 2D-CFAR
 *
 * @param[out] res  2D-CFAR检测结果
 * @param[in] magSepc2D  输入幅度谱
 * @param[in] cfg  2D-CFAR配置
 * @return 0 - success
 */
int radar_cfar2d_goca(cfar2d_result_t *res, const matrix2d_int32_t *magSepc2D, cfar2d_cfg_t *cfg)
{

#define A(i, j) magSepc2D->data[(i) * magSepc2D->tda1 + (wrap_pos(j, magSepc2D->size1))]
#define DIV_Q16(a, b) ((((int64_t)a << 32) / b) >> 16)

    RADAR_ASSERT(res->point != NULL);
    res->numPoint = 0;
    bool enable_sliding_window = cfg->numTrain[1] >= 2;
    const int M = magSepc2D->size0;
    const int N = magSepc2D->size1;
    const int total[2] = { cfg->numGuard[0] + cfg->numTrain[0], cfg->numGuard[1] + cfg->numTrain[1] };
    const int guard[2] = { cfg->numGuard[0], cfg->numGuard[1] };
    const int train[2] = { cfg->numTrain[0], cfg->numTrain[1] };
    const int32_t thSNR = (int32_t)(cfg->thSNR * (1 << 16));

    for (int idxR = 0; idxR < M; idxR++) {
        int win_pos = 0;
        for (int idxV = N / 2; idxV < N * 3 / 2; idxV++) {
            int32_t s = A(idxR, idxV);
            int64_t sumLeft, sumRight;
            /* 计算横向的噪声水平 */
            if (s < cfg->thAmp) {
                continue;
            } else if (enable_sliding_window || (win_pos + 1 != idxV)) {
                sumLeft = 0;
                sumRight = 0;
                for (int i = idxV - total[1]; i < idxV - guard[1]; i++) {
                    sumLeft += A(idxR, i);
                }
                for (int i = idxV + guard[1] + 1; i <= idxV + total[1]; i++) {
                    sumRight += A(idxR, i);
                }
            } else {
                sumLeft += A(idxR, idxV - guard[1] - 1) - A(idxR, idxV - total[1] - 1);
                sumRight += A(idxR, idxV + total[1]) - A(idxR, idxV + guard[1]);
            }
            win_pos = idxV;
            int32_t noise_th = DIV_Q16(s, thSNR);
            int32_t noise_level = ((sumLeft > sumRight) ? sumLeft : sumRight) / (train[1] == 0 ? 1 : train[1]);
            if (noise_level > noise_th) {
                continue;
            }

            /* 计算纵向的噪声水平 */
            int _idxV = idxV > N ? idxV - N : idxV;
            int64_t sumUp = 0, sumDown = 0;
            int start, end, count;

            start = idxR - total[0];
            end = start + train[0];
            if (start < 0)
                start = 0;
            count = 0;
            for (int i = start; i < end; i++) {
                sumUp += A(i, _idxV);
                count++;
            }
            if (count > 1) {
                sumUp /= count;
            }

            start = idxR + guard[0] + 1;
            end = start + train[0];
            if (end > M)
                end = M;
            count = 0;
            for (int i = start; i < end; i++) {
                sumDown += A(i, _idxV);
                count++;
            }
            if (count > 1) {
                sumDown /= count;
            }

            noise_level = ((sumUp > noise_level) ? sumUp : noise_level);
            noise_level = ((sumDown > noise_level) ? sumDown : noise_level);
            /*  添加点 */
            if (noise_level < noise_th) {
                if (res->numPoint < res->capacity) {
                    res->point[res->numPoint].idx0 = idxR;
                    res->point[res->numPoint].idx1 = _idxV;
                    res->point[res->numPoint].amp = s;
                    int64_t snr;
                    if (noise_level == 0) {
                        noise_level = 1;
                    }
                    snr = ((int64_t)s << 8) / noise_level;
                    if (snr > INT32_MAX) {
                        snr = INT32_MAX;
                    }
                    res->point[res->numPoint].snr = snr;
                    res->numPoint++;
                }
            }
        }
    }

#undef A
#undef DIV_Q16
    return 0;
}

/**
 * @brief GOCA-2D-CFAR 计算噪声水平，和radar_cfar2d_goca()逻辑一样，调试用
 *
 * @param noise 噪声矩阵
 * @param magSepc2D 幅度矩阵
 * @param cfg GOCA-2D-CFAR 配置
 * @return int
 */
int radar_cfar2d_goca_debug(matrix2d_int32_t *noise, const matrix2d_int32_t *magSepc2D, cfar2d_cfg_t *cfg)
{

#define A(i, j) magSepc2D->data[(i) * magSepc2D->tda1 + (wrap_pos(j, magSepc2D->size1))]
#define NOISE(i, j) noise->data[(i) * noise->tda1 + (wrap_pos(j, noise->size1))]
#define DIV_Q16(a, b) ((((int64_t)a << 32) / b) >> 16)

    RADAR_ASSERT(noise != NULL && magSepc2D != NULL && cfg != NULL);

    bool enable_sliding_window = cfg->numTrain[1] >= 2;
    const int M = magSepc2D->size0;
    const int N = magSepc2D->size1;
    const int total[2] = { cfg->numGuard[0] + cfg->numTrain[0], cfg->numGuard[1] + cfg->numTrain[1] };
    const int guard[2] = { cfg->numGuard[0], cfg->numGuard[1] };
    const int train[2] = { cfg->numTrain[0], cfg->numTrain[1] };
    const int32_t thSNR = (int32_t)(cfg->thSNR * (1 << 16));

    for (int idxR = 0; idxR < M; idxR++) {
        int win_pos = 0;
        for (int idxV = N / 2; idxV < N * 3 / 2; idxV++) {
            int32_t s = A(idxR, idxV);
            int64_t sumLeft, sumRight;

            /* 计算横向的噪声水平 */
            if (s < cfg->thAmp) {
                continue;
                NOISE(idxR, idxV) = 0;
            } else if (enable_sliding_window || (win_pos + 1 != idxV)) {
                sumLeft = 0;
                sumRight = 0;
                for (int i = idxV - total[1]; i < idxV - guard[1]; i++) {
                    sumLeft += A(idxR, i);
                }
                for (int i = idxV + guard[1] + 1; i <= idxV + total[1]; i++) {
                    sumRight += A(idxR, i);
                }
            } else {
                sumLeft += A(idxR, idxV - guard[1] - 1) - A(idxR, idxV - total[1] - 1);
                sumRight += A(idxR, idxV + total[1]) - A(idxR, idxV + guard[1]);
            }
            win_pos = idxV;
            int32_t noise_th = DIV_Q16(s, thSNR);
            int32_t noise_level = ((sumLeft > sumRight) ? sumLeft : sumRight) / (train[1] == 0 ? 1 : train[1]);
            if (noise_level > noise_th) {
                NOISE(idxR, idxV) = noise_level;
                continue;
            }

            /* 计算纵向的噪声水平 */
            int _idxV = idxV > N ? idxV - N : idxV;
            int64_t sumUp = 0, sumDown = 0;
            int start, end, count;

            start = idxR - total[0];
            end = start + train[0];
            if (start < 0)
                start = 0;
            count = 0;
            for (int i = start; i < end; i++) {
                sumUp += A(i, _idxV);
                count++;
            }
            if (count > 1) {
                sumUp /= count;
            }

            start = idxR + guard[0] + 1;
            end = start + train[0];
            if (end > M)
                end = M;
            count = 0;
            for (int i = start; i < end; i++) {
                sumDown += A(i, _idxV);
                count++;
            }
            if (count > 1) {
                sumDown /= count;
            }

            noise_level = ((sumUp > noise_level) ? sumUp : noise_level);
            noise_level = ((sumDown > noise_level) ? sumDown : noise_level);
            NOISE(idxR, idxV) = noise_level;
        }
    }

#undef A
#undef DIV_Q16
#undef SNR
    return 0;
}


/**
 * @brief 删除部分2D-CFAR检测结果
 *
 * @note CFAR检测结果中，一个目标可能会产生小范围内的多个点。
 *       部分点的信噪比较低，导致后续计算角度时的精度很低，
 *       会导致后续在二维平面上聚类时无法将这些点分配正确的簇上
 *       因此要提前删除这些点。
 *
 * @details 当点A和点B的距离小对于range，且A < th * B 时，删除点A。
 *          在程序中使用乘法代替除法，比较公式为 A * r < B, 其中 r = 1/th。
 *
 * @warning 维度1方向上的距离计算的是解周期后的距离，即对于范围[0,8), 0和7点距离是1而不是7
 *          因此在使用radar_cfar2d_goca()函数的时候就要确保输入的幅度谱是一个维度依次为[距离，速度]的二维矩阵。
 *
 * @warning 输入的检测结果要确保idx0是有序的，res->point[i].idx0 <= res->point[i + 1].idx0
 *          radar_cfar2d_goca()函数输出的结果是满足要求的，使用有序的数据结构可以提高效率
 *
 * @param res  CFAR检测结果
 * @param cfg  CFAR筛选配置
 *               range0 ：维度0的距离；
 *               range1：维度1的距离；
 *               shape1：维度1的长度;
 *               thSNR：SNR阈值；
 * @return int
 */
int radar_cfar_result_filtering(cfar2d_result_t *res, const cfar2d_filter_cfg_t *cfg)
{
    const size_t n = res->numPoint;
    cfar2d_point_t *p = res->point;
    cbits mask = cbits_with_size(n, true);
    int32_t r_q16 = div_i32q16_i32q16(1 << 16, (int32_t)(cfg->thSNR * (1 << 16)));
    for (size_t i = 0; i < n; i++) {
        cfar2d_point_t *a = &p[i];
        int64_t ar_q16 = (int64_t)a->amp * r_q16;
        for (size_t j = 0; j < n; j++) {
            if (i == j)
                continue;
            cfar2d_point_t *b = &p[j];
            if (b->idx0 + cfg->range0 < a->idx0) {
                continue;
            }
            if (b->idx0 > a->idx0 + cfg->range0) {
                break;
            }
            uint16_t diff;
            diff = abs_diff(a->idx1, b->idx1);
            if (diff > cfg->shape1 / 2) {
                diff = cfg->shape1 - diff;
            }
            if (diff <= cfg->range1) {
                if (ar_q16 < ((int64_t)b->amp << 16)) {
                    cbits_reset(&mask, i);
                    break;
                }
            }
        }
    }
    /* 删除掉不满足条件的点 */
    size_t numPoint = 0;
    for (size_t i = 0; i < n; i++) {
        if (cbits_test(&mask, i)) {
            if (numPoint != i)
                p[numPoint] = p[i];
            numPoint++;
        }
    }
    res->numPoint = numPoint;
    cbits_drop(&mask);
    return 0;
}

#define MASK_AND 0 // 仅仅在周期为2的幂的情况下使用
#define IF_ELSE 1
#define REMAINDER 2

#define INDEX_UNWRAPPER_METHOD IF_ELSE

#if INDEX_UNWRAPPER_METHOD == MASK_AND // 选择unwrapper方式
static inline int unwrapper_neg(int x, int n)
{
    return x & (n - 1);
}
static inline int wrap_pos(int x, int n)
{
    return x & (n - 1);
}
#elif INDEX_UNWRAPPER_METHOD == IF_ELSE   // 选择unwrapper方式

static inline int unwrapper_neg(int x, int n)
{
    return x < 0 ? x + n : x;
}
static inline int wrap_pos(int x, int n)
{
    return x >= n ? x - n : x;
}
#elif INDEX_UNWRAPPER_METHOD == REMAINDER // 选择unwrapper方式
static inline int unwrapper_neg(int x, int n)
{
    return (x + n) % n;
}
static inline int wrap_pos(int x, int n)
{
    return x % n;
}
#endif

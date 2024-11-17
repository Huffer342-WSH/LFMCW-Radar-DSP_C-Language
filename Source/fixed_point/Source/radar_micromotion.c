#include "radar_micromotion.h"

#include <stdlib.h>

#include "radar_error.h"

int radar_micromotion_handle_init(radar_micromotion_handle_t *mm, size_t numRangeBin, size_t capacity)
{
    mm->numRangeBin = numRangeBin;
    mm->capacity = capacity;
    mm->in = 0;
    mm->numFrame = 0;
    mm->prevFramePhase = radar_matrix2d_int16_alloc(numRangeBin, 1);
    if (mm->prevFramePhase == NULL) {
        RADAR_ERROR("radar_micromotion_handle_init() failed to allocate space for prevFramePhase", RADAR_ENOMEM);
        return -1;
    }
    mm->deltaPhase = radar_matrix2d_int16_alloc(numRangeBin, capacity);
    if (mm->deltaPhase == NULL) {
        free(mm->prevFramePhase);
        RADAR_ERROR("radar_micromotion_handle_init() failed to allocate space for deltaPhase", RADAR_ENOMEM);
        return -2;
    }
    return 0;
}


void radar_micromotion_handle_deinit(radar_micromotion_handle_t *mm)
{
    radar_matrix2d_int16_free(mm->prevFramePhase);
    radar_matrix2d_int16_free(mm->deltaPhase);
    mm->in = 0;
}

/**
 * @brief 添加一帧信息
 *
 * @param mm
 * @param rdms
 * @return int
 *
 * @details 遍历每一个距离单元，计算这一帧数据0速度的相位。
 *          和上一帧相位比较得到相位差，保存到deltaPhase中，新计算的相位保存到prevFramePhase中
 *          其中幅度较小的点的相位差记为0
 */
void radar_micromotion_add_frame(radar_micromotion_handle_t *mmhandle, matrix3d_complex_int16_t *rdms)
{
}

/**
 * @brief 查询一个距离单元的微动信息
 *
 * @param queue
 * @param rangeBin
 * @return radar_micromotion_target_info_t
 *
 * @details 遍历指定距离单元的相位差，得到相位-时间曲线。遍历过程中统计相位最大值最小值以及绝对值的累加和，作为微动信息
 */
radar_micromotion_target_info_t radar_micromotion_check_rangebin(radar_micromotion_handle_t *mmhandle, size_t rangeBin)
{
    radar_micromotion_target_info_t res;

    return res;
}

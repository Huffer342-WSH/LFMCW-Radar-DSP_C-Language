#include "radar_micromotion.h"

#include <stdlib.h>

radar_micromotion_handle_t *radar_micromotion_queue_alloc(size_t numRangeBin, size_t capacity)
{
    radar_micromotion_handle_t *queue = (radar_micromotion_handle_t *)malloc(sizeof(radar_micromotion_handle_t));

    queue->numRangeBin = numRangeBin;
    queue->capacity = capacity;
    queue->in = 0;
    queue->numFrame = 0;
    queue->prevFrame = radar_matrix2d_complex_int16_alloc(numRangeBin, 1);
    queue->phaseChange = radar_matrix2d_int16_alloc(numRangeBin, capacity);
    return queue;
}


/**
 * @brief 添加一帧信息
 *
 * @param queue
 * @param rdm
 * @return int
 */
int radar_micromotion_add_frame(radar_micromotion_handle_t *queue, matrix2d_complex_int16_t *rdm)
{
    return 0;
}

/**
 * @brief 查询一个距离单元的微动信息
 *
 * @param queue
 * @param rangeBin
 * @return radar_micromotion_target_info_t
 */
radar_micromotion_target_info_t radar_micromotion_check_rangebin(radar_micromotion_handle_t *queue, size_t rangeBin)
{
    radar_micromotion_target_info_t res;

    return res;
}

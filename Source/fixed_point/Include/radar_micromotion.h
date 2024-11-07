#ifndef _RADAR_micromotion_H_
#define _RADAR_micromotion_H_

#include "radar_math_types.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 微动信息循环队列
 *
 */
typedef struct {
    uint16_t numRangeBin;                // 距离单元数量
    uint16_t capacity;                   // 容量,指最多能保存多少帧数据
    uint16_t in;                         // 输入端指针，指向下一帧数据要存放的位置
    uint16_t numFrame;                   // 内部已经存放的帧数
    matrix2d_complex_int16_t *prevFrame; // 上一帧的RDM中0速度对应的一列数据
    matrix2d_int16_t *phaseChange;       // 保存所有距离单元的一段时间的相位变化
} radar_micromotion_handle_t;


typedef struct {
    uint16_t rangeBin;
    uint16_t duration;  // 帧数
    uint32_t distance;  // 微动总路程
    uint32_t amplitude; // 微动幅度（最大值-最小值）
} radar_micromotion_target_info_t;


#ifdef __cplusplus
}
#endif
#endif /* _RADAR_micromotion_H_ */

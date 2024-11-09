/**
 * @file radar_math_types.h
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 定义变量数据类型
 *
 *
 * @note 定点数版本的程序大部分计算都需要使用定点数完成，小部分计算为了保证有效位数可以使用浮点数。
 *
 *
 * @version 0.1
 * @date 2024-11-05
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef _RADAR_MATH_TYPES_H_
#define _RADAR_MATH_TYPES_H_

#include <stdint.h>
#include <stddef.h>

typedef float rd_float_t;

typedef int8_t q7_t;
typedef int16_t q15_t;
typedef int32_t q31_t;
typedef int64_t q63_t;
typedef uint8_t uq7_t;
typedef uint16_t uq16_t;
typedef uint32_t uq32_t;
typedef uint64_t uq64_t;

#include "radar_matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
#endif /* _RADAR_MATH_TYPES_H_ */

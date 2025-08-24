/**
 * @file radar_math_types.hh
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief C++部分代码中使用的数据类型
 * @version 0.1
 * @date 2025-08-24
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include <radar/common/radar_math_types.h>
#include <Eigen/Dense>

typedef Eigen::Matrix<rd_float_t, 3, 1> Vector3r;
typedef Eigen::Matrix<rd_float_t, 3, 3> Matrix33r;
typedef Eigen::Matrix<rd_float_t, 3, 4> Matrix34r;
typedef Eigen::Matrix<rd_float_t, 4, 1> Vector4r;
typedef Eigen::Matrix<rd_float_t, 4, 3> Matrix43r;
typedef Eigen::Matrix<rd_float_t, 4, 4> Matrix44r;

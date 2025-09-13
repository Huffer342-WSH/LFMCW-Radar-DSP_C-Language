/**
 * @file pyradar_fixed_radar_clutter_filter.hh
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 杂波滤波器模块绑定
 * @version 0.1
 * @date 2025-09-13
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <pybind11/pybind11.h>

namespace py = pybind11;

void bind_radar_clutter_filter(py::module_ &m);

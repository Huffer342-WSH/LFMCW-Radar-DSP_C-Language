
#include <pybind11/numpy.h>
#include <pybind11/stl.h>


#include "matrix_wrapper.hh"
#include "pyradar_fixed_radar_cfar.hh"

#include "radar_cfar.h"


void bind_cfar2d_point(pybind11::module_ &m)
{
    pybind11::class_<cfar2d_point_t>(m, "CFAR2DPoint")
        .def(pybind11::init<>())
        .def_readwrite("idx0", &cfar2d_point_t::idx0)
        .def_readwrite("idx1", &cfar2d_point_t::idx1)
        .def_readwrite("amp", &cfar2d_point_t::amp)
        .def_readwrite("snr", &cfar2d_point_t::snr); // snr 是 8 位小数
}


void bind_cfar2d_result(pybind11::module_ &m)
{
    pybind11::class_<cfar2d_result_t>(m, "CFAR2DResult")
        .def(pybind11::init<>()) // 默认构造函数
        .def_readwrite("numMax", &cfar2d_result_t::numMax)
        .def_readwrite("numPoint", &cfar2d_result_t::numPoint)
        .def_readwrite("owner", &cfar2d_result_t::owner)
        .def_property(
            "point",
            [](cfar2d_result_t &self) {
                pybind11::list points;
                for (size_t i = 0; i < self.numPoint; ++i) {
                    points.append(pybind11::cast(self.point + i)); // 将 C 数组元素封装成 Python 对象
                }
                return points;
            },
            [](cfar2d_result_t &self, const pybind11::list &points) {
                if (self.point == nullptr) {
                    throw std::runtime_error("Invalid state: point is nullptr");
                }
                if (points.size() > self.numMax) {
                    throw std::runtime_error("Invalid state: too many points");
                }
                self.numPoint = points.size();
                for (size_t i = 0; i < self.numPoint; ++i) {
                    // 将 Python 列表中的元素复制到 C 数组
                    self.point[i] = pybind11::cast<cfar2d_point_t>(points[i]);
                }
            }
        );
}


void bind_cfar2d_cfg(pybind11::module_ &m)
{
    pybind11::class_<cfar2d_cfg_t>(m, "CFAR2DCfg")
        .def(pybind11::init<>()) // 默认构造函数
        .def_property(
            "numTrain",
            [](cfar2d_cfg_t &self) -> pybind11::array_t<double> {
                std::vector<size_t> shape = { 2 };
                return array_c2numpy<uint16_t>(self.numTrain, shape);
            },
            [](cfar2d_cfg_t &self, pybind11::array_t<uint16_t, pybind11::array::c_style | pybind11::array::forcecast> arr) {
                std::vector<size_t> shape = { 2 };
                array_numpy2c<uint16_t>(self.numTrain, arr, shape);
            }
        )
        .def_property(
            "numGuard",
            [](cfar2d_cfg_t &self) -> pybind11::array_t<double> {
                std::vector<size_t> shape = { 2 };
                return array_c2numpy<uint16_t>(self.numGuard, shape);
            },
            [](cfar2d_cfg_t &self, pybind11::array_t<uint16_t, pybind11::array::c_style | pybind11::array::forcecast> arr) {
                std::vector<size_t> shape = { 2 };
                array_numpy2c<uint16_t>(self.numGuard, arr, shape);
            }
        )
        .def_readwrite("thSNR", &cfar2d_cfg_t::thSNR)
        .def_readwrite("thAmp", &cfar2d_cfg_t::thAmp);
}


void bind_cfar2d_filter_cfg(pybind11::module_ &m)
{
    pybind11::class_<cfar2d_filter_cfg_t>(m, "CFAR2DFilterCfg")
        .def(pybind11::init<>()) // 默认构造函数
        .def_readwrite("range0", &cfar2d_filter_cfg_t::range0)
        .def_readwrite("range1", &cfar2d_filter_cfg_t::range1)
        .def_readwrite("thSNR", &cfar2d_filter_cfg_t::thSNR);
}


void bind_cfar(pybind11::module_ &m)
{
    bind_cfar2d_point(m);
    bind_cfar2d_result(m);
    bind_cfar2d_cfg(m);
    bind_cfar2d_filter_cfg(m); // 添加对 cfar2d_filter_cfg_t 的绑定
}


#include <pybind11/numpy.h>
#include <pybind11/stl.h>


#include <matrix_wrapper.hh>
#include <list_wrapper.hh>

#include "pyradar_fixed_radar_cfar.hh"

#include "radar_cfar.h"


void bind_cfar2d_point(pybind11::module_ &m)
{
    PYBIND11_NUMPY_DTYPE(cfar2d_point_t, idx0, idx1, amp, snr);
    pybind11::class_<cfar2d_point_t>(m, "cfar2d_point")
        .def(pybind11::init<>())
        .def_readwrite("idx0", &cfar2d_point_t::idx0)
        .def_readwrite("idx1", &cfar2d_point_t::idx1)
        .def_readwrite("amp", &cfar2d_point_t::amp)
        .def_readwrite("snr", &cfar2d_point_t::snr)
        .def("to_numpy", [](cfar2d_point_t &self) {
            return array_c2numpy<cfar2d_point_t>(&self, { 1 });
        }); // snr 是 8 位小数
}


void bind_cfar2d_result(pybind11::module_ &m)
{
    pybind11::class_<cfar2d_result_t>(m, "cfar2d_result")
        .def(pybind11::init<>()) // 默认构造函数
        .def_readwrite("numMax", &cfar2d_result_t::numMax)
        .def_readwrite("numPoint", &cfar2d_result_t::numPoint)
        .def_readwrite("owner", &cfar2d_result_t::owner)
        .def_property(
            "point", //
            [](cfar2d_result_t &self) {
                return array_c2numpy<cfar2d_point_t>(self.point, { self.numMax });
            }, //
            [](cfar2d_result_t &self, pybind11::array arr) {
                array_numpy2c<cfar2d_point_t>(self.point, arr, { self.numMax });
            } //
        )
        .def("getList", [](cfar2d_result_t &self) {
            return convert_cstyle_struct_array_to_pylist<cfar2d_point_t>(self.point, self.numPoint);
        });
}


void bind_cfar2d_cfg(pybind11::module_ &m)
{


    pybind11::class_<cfar2d_cfg_t>(m, "cfar2d_cfg")
        .def(pybind11::init<>()) // 默认构造函数
        .def_property(
            "numTrain",
            [](cfar2d_cfg_t &self) -> pybind11::array_t<double> {
                std::vector<size_t> shape = { 2 };
                return array_c2numpy<uint16_t>(self.numTrain, shape);
            },
            [](cfar2d_cfg_t &self,
               pybind11::array_t<uint16_t, pybind11::array::c_style | pybind11::array::forcecast> arr) {
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
            [](cfar2d_cfg_t &self,
               pybind11::array_t<uint16_t, pybind11::array::c_style | pybind11::array::forcecast> arr) {
                std::vector<size_t> shape = { 2 };
                array_numpy2c<uint16_t>(self.numGuard, arr, shape);
            }
        )
        .def_readwrite("thSNR", &cfar2d_cfg_t::thSNR)
        .def_readwrite("thAmp", &cfar2d_cfg_t::thAmp);
}


void bind_cfar2d_filter_cfg(pybind11::module_ &m)
{
    pybind11::class_<cfar2d_filter_cfg_t>(m, "cfar2d_filter_cfg")
        .def(pybind11::init<>()) // 默认构造函数
        .def_readwrite("range0", &cfar2d_filter_cfg_t::range0)
        .def_readwrite("range1", &cfar2d_filter_cfg_t::range1)
        .def_readwrite("thSNR", &cfar2d_filter_cfg_t::thSNR);
}

void bind_cfar2d_result_func(pybind11::module_ &m)
{
    m.def("cfar2d_result_alloc", &cfar2d_result_alloc, "Allocates a cfar2d_result_t object");
    m.def("cfar2d_result_free", &cfar2d_result_free, "Frees a cfar2d_result_t object");
}

void bind_radar_cfar2d_goca(pybind11::module_ &m)
{
    m.def("radar_cfar2d_goca", &radar_cfar2d_goca, "2D-CFAR");
}

void bind_radar_cfar_result_filtering(pybind11::module_ &m)
{
    m.def("radar_cfar_result_filtering", &radar_cfar_result_filtering, "Filter 2D-CFAR results");
}

void bind_cfar(pybind11::module_ &m)
{
    bind_cfar2d_point(m);
    bind_cfar2d_result(m);
    bind_cfar2d_cfg(m);
    bind_cfar2d_filter_cfg(m); // 添加对 cfar2d_filter_cfg_t 的绑定

    bind_cfar2d_result_func(m);
    bind_radar_cfar2d_goca(m);
    bind_radar_cfar_result_filtering(m);
}
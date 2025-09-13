#include "pyradar_fixed_radar_clutter_filter.hh"

#include <radar/sp/fixed_point/radar_clutter_filter.h>

#include <matrix_wrapper.hh>
#include <pybind11/numpy.h>

void bind_radar_clutter_filter(py::module_ &m)
{
    // 1. 绑定配置结构体 radar_clutter_filter_cfg_t
    py::class_<radar_clutter_filter_cfg_t>(m, "RadarClutterFilterCfg")
        .def(py::init<>())
        .def_readwrite("n_channel", &radar_clutter_filter_cfg_t::n_channel)
        .def_readwrite("n_rb", &radar_clutter_filter_cfg_t::n_rb)
        .def_readwrite("fifo_len", &radar_clutter_filter_cfg_t::fifo_len)
        .def_readwrite("weight_max", &radar_clutter_filter_cfg_t::weight_max)
        .def_readwrite("weight_min", &radar_clutter_filter_cfg_t::weight_min)
        .def_readwrite("phase_max", &radar_clutter_filter_cfg_t::phase_max)
        .def_readwrite("phase_min", &radar_clutter_filter_cfg_t::phase_min)
        .def_readwrite("update_interval", &radar_clutter_filter_cfg_t::update_interval);

    // 2. 绑定 radar_clutter_filter_t (不直接暴露成员，只暴露操作)
    py::class_<radar_clutter_filter_t>(m, "RadarClutterFilter")
        .def(
            "filter",
            [](radar_clutter_filter_t *self, matrix3d_complex_int16_t *rdm) {
                return radar_static_clutter_filter(self, rdm);
            },
            py::arg("rdm"));

    // 3. 工厂函数 new/delete
    m.def(
        "radar_static_clutter_filter_new",
        [](const radar_clutter_filter_cfg_t &cfg) {
            radar_clutter_filter_t *ptr = radar_static_clutter_filter_new(&cfg);
            if (!ptr)
                throw std::runtime_error("Failed to create radar clutter filter");
            return ptr;
        },
        py::return_value_policy::reference, py::arg("cfg"));

    m.def(
        "radar_static_clutter_filter_delete",
        [](radar_clutter_filter_t *filter) {
            radar_static_clutter_filter_delete(filter);
        },
        py::arg("filter"));
}

#include "pyradar_fixed_track.hh"

#include "track.h"


void bind_tracker_config(pybind11::module_ &m)
{
    pybind11::class_<tracker_config_t>(m, "TrackerConfig")
        .def(pybind11::init<>())                                                      // 默认构造函数
        .def_readwrite("velocity_noise_coef", &tracker_config_t::velocity_noise_coef) // 绑定 velocity_noise_coef 属性
        .def_readwrite("sigma_phi", &tracker_config_t::sigma_phi)                     // 绑定 sigma_phi 属性
        .def_readwrite("sigma_r", &tracker_config_t::sigma_r)                         // 绑定 sigma_r 属性
        .def_readwrite("sigma_r_dot", &tracker_config_t::sigma_r_dot)                 // 绑定 sigma_r_dot 属性
        .def_readwrite("missed_distance", &tracker_config_t::missed_distance)         // 绑定 missed_distance 属性
        .def("__repr__", [](const tracker_config_t &cfg) {
            return "<TrackerConfig velocity_noise_coef=" + std::to_string(cfg.velocity_noise_coef) + ", sigma_phi=" + std::to_string(cfg.sigma_phi) +
                   ", sigma_r=" + std::to_string(cfg.sigma_r) + ", sigma_r_dot=" + std::to_string(cfg.sigma_r_dot) +
                   ", missed_distance=" + std::to_string(cfg.missed_distance) + ">";
        }); // 提供一个 __repr__ 方法以便更好地显示对象信息
}

void bind_track(pybind11::module_ &m)
{
    bind_tracker_config(m);
}

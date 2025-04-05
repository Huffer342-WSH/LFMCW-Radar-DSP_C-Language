#include "pyradar_fixed_track.hh"
#include "matrix_wrapper.hh"
#include <radar/ot/track.h>
#include <radar/ot/track_target.h>


void bind_tracker_config(pybind11::module_ &m)
{
    pybind11::class_<tracker_config_t>(m, "TrackerConfig")
        .def(pybind11::init<>())
        .def_readwrite("velocity_noise_coef", &tracker_config_t::velocity_noise_coef)
        .def_readwrite("sigma_phi", &tracker_config_t::sigma_phi)
        .def_readwrite("sigma_r", &tracker_config_t::sigma_r)
        .def_readwrite("sigma_r_dot", &tracker_config_t::sigma_r_dot)
        .def_readwrite("missed_distance", &tracker_config_t::missed_distance)
        .def_readwrite("unassociated_time", &tracker_config_t::unassociated_time)
        .def_readwrite("keep_motion_time", &tracker_config_t::keep_motion_time)
        .def_readwrite("keep_static_time", &tracker_config_t::keep_static_time)
        .def_readwrite("speed_threshold", &tracker_config_t::speed_threshold)
        .def_readwrite("missed_probability", &tracker_config_t::missed_probability)
        .def("__repr__", [](const tracker_config_t &cfg) {
            return "<TrackerConfig velocity_noise_coef=" + std::to_string(cfg.velocity_noise_coef) + ", sigma_phi=" + std::to_string(cfg.sigma_phi) +
                   ", sigma_r=" + std::to_string(cfg.sigma_r) + ", sigma_r_dot=" + std::to_string(cfg.sigma_r_dot) +
                   ", missed_distance=" + std::to_string(cfg.missed_distance) + ", unassociated_time=" + std::to_string(cfg.unassociated_time) +
                   ", keep_motion_time=" + std::to_string(cfg.keep_motion_time) + ", keep_static_time=" + std::to_string(cfg.keep_static_time) +
                   ", speed_threshold=" + std::to_string(cfg.speed_threshold) + ", missed_probability=" + std::to_string(cfg.missed_probability) + ">";
        });
}


void bind_track(pybind11::module_ &m)
{
    bind_tracker_config(m);
}

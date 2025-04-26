#include "pyradar_fixed_track.hh"
#include "matrix_wrapper.hh"
#include <radar/ot/track.h>
#include <radar/ot/track_target.h>

#include "pybind11/stl.h"


void bind_tracker_config(pybind11::module_ &m)
{
    pybind11::class_<tracker_config_t>(m, "TrackerConfig")
        .def(pybind11::init<>())
        .def_readwrite("velocity_noise_coef", &tracker_config_t::velocity_noise_coef)
        .def_readwrite("sigma_phi", &tracker_config_t::sigma_phi)
        .def_readwrite("sigma_r", &tracker_config_t::sigma_r)
        .def_readwrite("sigma_r_dot", &tracker_config_t::sigma_r_dot)
        .def_readwrite("missed_distance", &tracker_config_t::missed_distance)
        .def_readwrite("time2delete_initiator", &tracker_config_t::time2delete_initiator)
        .def_readwrite("time2init_motion", &tracker_config_t::time2init_motion)
        .def_readwrite("time2init_static", &tracker_config_t::time2init_static)
        .def_readwrite("speed_threshold", &tracker_config_t::speed_threshold)
        .def_readwrite("time2delete_deleter", &tracker_config_t::time2delete_deleter)
        .def_readwrite("time2stop_unassociated", &tracker_config_t::time2stop_unassociated)
        .def_readwrite("missed_probability", &tracker_config_t::missed_probability)
        .def_property(
            "fov",
            [](tracker_config_t &self) {
                return array_c2numpy<rd_float_t>(self.fov, { 2 });
            },
            [](tracker_config_t &self, pybind11::array &arr) {
                array_numpy2c<rd_float_t>(self.fov, arr, { 2 });
            })
        .def_property(
            "radius_range",
            [](tracker_config_t &self) {
                return array_c2numpy<rd_float_t>(self.radius_range, { 2 });
            },
            [](tracker_config_t &self, pybind11::array &arr) {
                array_numpy2c<rd_float_t>(self.radius_range, arr, { 2 });
            })
        .def("__repr__", [](const tracker_config_t &cfg) {
            return "<TrackerConfig velocity_noise_coef=" + std::to_string(cfg.velocity_noise_coef) + ", sigma_phi=" + std::to_string(cfg.sigma_phi) +
                   ", sigma_r=" + std::to_string(cfg.sigma_r) + ", sigma_r_dot=" + std::to_string(cfg.sigma_r_dot) +
                   ", missed_distance=" + std::to_string(cfg.missed_distance) + ", time2delete_initiator=" + std::to_string(cfg.time2delete_initiator) +
                   ", time2init_motion=" + std::to_string(cfg.time2init_motion) + ", time2init_static=" + std::to_string(cfg.time2init_static) +
                   ", speed_threshold=" + std::to_string(cfg.speed_threshold) + ", time2delete_deleter=" + std::to_string(cfg.time2delete_deleter) +
                   ", time2stop_unassociated=" + std::to_string(cfg.time2stop_unassociated) + ", missed_probability=" + std::to_string(cfg.missed_probability) +
                   ">";
        });
}


void bind_track(pybind11::module_ &m)
{
    bind_tracker_config(m);
}

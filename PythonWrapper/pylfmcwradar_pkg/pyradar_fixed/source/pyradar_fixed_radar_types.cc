#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pyradar_fixed_radar_types.hh>

#include <stdexcept>

#include <matrix_wrapper.hh>
#include <list_wrapper.hh>

#include "radar_types.h"


namespace py = pybind11;


void bind_measurements(pybind11::module_ &m)
{

    PYBIND11_NUMPY_DTYPE(radar_measurements_fixed_t, distance, velocity, azimuth, sin_azimuth, amp, snr);
    pybind11::class_<radar_measurements_fixed_t>(m, "measurements")
        .def(pybind11::init<>())
        .def_readwrite("distance", &radar_measurements_fixed_t::distance)
        .def_readwrite("velocity", &radar_measurements_fixed_t::velocity)
        .def_readwrite("azimuth", &radar_measurements_fixed_t::azimuth)
        .def_readwrite("sin_azimuth", &radar_measurements_fixed_t::sin_azimuth)
        .def_readwrite("amp", &radar_measurements_fixed_t::amp)
        .def_readwrite("snr", &radar_measurements_fixed_t::snr)
        .def("to_numpy",
             [](radar_measurements_fixed_t &self) {
                 return array_c2numpy<radar_measurements_fixed_t>(&self, { 1 });
             })
        .def("__repr__", [](const radar_measurements_fixed_t &m) {
            return "<measurements(distance=" + std::to_string(m.distance) + ", velocity=" + std::to_string(m.velocity) +
                   ", azimuth=" + std::to_string(m.azimuth) + ", amp=" + std::to_string(m.amp) +
                   ", snr=" + std::to_string(m.snr) + ")>";
        });

    ;
}


void bind_measurements_list(pybind11::module_ &m)
{
    pybind11::class_<radar_measurement_list_fixed_t>(m, "RadarMeasurementList")
        .def(pybind11::init<>()) // 默认构造函数
        .def_readwrite("num", &radar_measurement_list_fixed_t::num)
        .def_readwrite("capacity", &radar_measurement_list_fixed_t::capacity)
        .def_property(
            "meas", //
            [](radar_measurement_list_fixed_t &self) {
                return array_c2numpy<radar_measurements_fixed_t>(self.meas, { self.capacity });
            }, //
            [](radar_measurement_list_fixed_t &self, py::array arr) {
                array_numpy2c<radar_measurements_fixed_t>(self.meas, arr, { self.capacity });
            } //
            )
        .def("getList", [](radar_measurement_list_fixed_t &self) {
            return convert_cstyle_struct_array_to_pylist(self.meas, self.num);
        });
}


void bind_radar_param(pybind11::module_ &m)
{
    PYBIND11_NUMPY_DTYPE(
        radar_param_t, wavelength, bandwidth, timeChrip, timeChripGap, timeFrameGap, numChannel, numSample, numRangeBin,
        numChrip, timeFrameVaild, resRange, resVelocity
    );

    pybind11::class_<radar_param_t>(m, "radar_param")
        .def(pybind11::init<>())
        .def_readwrite("wavelength", &radar_param_t::wavelength)
        .def_readwrite("bandwidth", &radar_param_t::bandwidth)
        .def_readwrite("timeChrip", &radar_param_t::timeChrip)
        .def_readwrite("timeChripGap", &radar_param_t::timeChripGap)
        .def_readwrite("timeFrameGap", &radar_param_t::timeFrameGap)
        .def_readwrite("numChannel", &radar_param_t::numChannel)
        .def_readwrite("numSample", &radar_param_t::numSample)
        .def_readwrite("numRangeBin", &radar_param_t::numRangeBin)
        .def_readwrite("numChrip", &radar_param_t::numChrip)
        .def_readwrite("timeFrameVaild", &radar_param_t::timeFrameVaild)
        .def_readwrite("resRange", &radar_param_t::resRange)
        .def_readwrite("resVelocity", &radar_param_t::resVelocity)
        .def("to_numpy", [](radar_param_t &self) {
            return array_c2numpy<radar_param_t>(&self, { 1 });
        });
}

void bind_radar_config(pybind11::module_ &m)
{

    pybind11::class_<radar_config_t>(m, "radar_config")
        .def(pybind11::init<>())
        .def_readwrite("cfarCfg", &radar_config_t::cfarCfg)
        .def_readwrite("cfar_filter_cfg", &radar_config_t::cfar_filter_cfg);
}

void bind_radar_basic_data(pybind11::module_ &m)
{
    pybind11::class_<radar_basic_data_t>(m, "radar_basic_data")
        .def(pybind11::init<>())
        .def_readwrite("param", &radar_basic_data_t::param)
        .def_readwrite("rdms", &radar_basic_data_t::rdms)
        .def_readwrite("staticClutter", &radar_basic_data_t::staticClutter)
        .def_readwrite("staticClutterAccBuffer", &radar_basic_data_t::staticClutterAccBuffer)
        .def_readwrite("magSpec2D", &radar_basic_data_t::magSpec2D);
}

void bind_radar_handle(pybind11::module_ &m)
{
    pybind11::class_<radar_handle_t>(m, "radar_handle")
        .def(pybind11::init<>())
        .def_readwrite("cntFrame", &radar_handle_t::cntFrame)
        .def_readwrite("param", &radar_handle_t::param)
        .def_readwrite("config", &radar_handle_t::config)
        .def_readwrite("basic", &radar_handle_t::basic)
        .def_readwrite("cfar", &radar_handle_t::cfar)
        .def_readwrite("meas", &radar_handle_t::meas)
        .def(
            "getMagSpec2D",
            [](radar_handle_t &self) {
                std::vector<size_t> shape = { self.param.numRangeBin, self.param.numChrip };
                pybind11::array_t<int32_t> numpy_array = array_c2numpy<int32_t>(self.basic.magSpec2D->data, shape);
                return numpy_array.attr("copy")();
            },
            "Get a copy of the 2D magnitude spectrum."
        );
}


void bind_radar_basic_data_init(pybind11::module_ &m)
{
    m.def(
        "radar_basic_data_init", &radar_basic_data_init, pybind11::return_value_policy::take_ownership,
        "Allocate a new radar_basic_data."
    );
}

void bind_radar_measurement_list_alloc(pybind11::module_ &m)
{
    m.def(
        "radar_measurement_list_alloc", &radar_measurement_list_alloc, pybind11::return_value_policy::take_ownership,
        "Allocate a new RadarMeasurementList with the specified capacity."
    );
}


void bind_radar_types(pybind11::module_ &m)
{
    bind_measurements(m);
    bind_measurements_list(m);
    bind_radar_param(m);
    bind_radar_config(m);
    bind_radar_basic_data(m);
    bind_radar_handle(m);
    bind_radar_basic_data_init(m);
    bind_radar_measurement_list_alloc(m);
}

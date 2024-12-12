#include "pyradar_fixed_radar_processer.hh"

#include <matrix_wrapper.hh>

#include "radar_processer.h"

#include <iomanip>

void bind_radar_init_param(pybind11::module_ &m)
{

    PYBIND11_NUMPY_DTYPE(radar_init_param_t, wavelength, bandwidth, timeChrip, timeChripGap, timeFrameGap, numChannel, numRangeBin, numChrip);
    pybind11::class_<radar_init_param_t>(m, "radar_init_param")
        .def(pybind11::init<>())
        .def_readwrite("wavelength", &radar_init_param_t::wavelength)
        .def_readwrite("bandwidth", &radar_init_param_t::bandwidth)
        .def_readwrite("rx_antenna_spacing", &radar_init_param_t::rx_antenna_spacing)
        .def_readwrite("timeChrip", &radar_init_param_t::timeChrip)
        .def_readwrite("timeChripGap", &radar_init_param_t::timeChripGap)
        .def_readwrite("timeFrameGap", &radar_init_param_t::timeFrameGap)
        .def_readwrite("numChannel", &radar_init_param_t::numChannel)
        .def_readwrite("numRangeBin", &radar_init_param_t::numRangeBin)
        .def_readwrite("numChrip", &radar_init_param_t::numChrip)
        .def_readwrite("numMaxCfarPoints", &radar_init_param_t::numMaxCfarPoints)
        .def_readwrite("numMaxCachedFrame", &radar_init_param_t::numMaxCachedFrame)
        .def_readwrite("numInitialMultiMeas", &radar_init_param_t::numInitialMultiMeas)
        .def_readwrite("numInitialCluster", &radar_init_param_t::numInitialCluster)
        .def("to_numpy",
             [](radar_init_param_t &self) {
                 return array_c2numpy<radar_init_param_t>(&self, { 1 });
             })
        .def("__repr__", [](const radar_init_param_t &self) {
            std::ostringstream oss;
            oss << "radar_init_param:\n"
                << std::setw(25) << "wavelength       =" << std::setw(3) << self.wavelength << "\n"
                << std::setw(25) << "bandwidth        =" << std::setw(3) << self.bandwidth << "\n"
                << std::setw(25) << "timeChrip        =" << std::setw(3) << self.timeChrip << "\n"
                << std::setw(25) << "timeChripGap     =" << std::setw(3) << self.timeChripGap << "\n"
                << std::setw(25) << "timeFrameGap     =" << std::setw(3) << self.timeFrameGap << "\n"
                << std::setw(25) << "numChannel       =" << std::setw(3) << self.numChannel << "\n"
                << std::setw(25) << "numRangeBin      =" << std::setw(3) << self.numRangeBin << "\n"
                << std::setw(25) << "numChrip         =" << std::setw(3) << self.numChrip << "\n"
                << std::setw(25) << "numMaxCfarPoints =" << std::setw(3) << self.numMaxCfarPoints << "\n";
            return oss.str();
        });
    ;
}

void bind_radar_functions(pybind11::module_ &m)
{
    // 绑定 radardsp_init 函数
    m.def("radardsp_init", &radardsp_init, "Initialize the radar system", pybind11::arg("radar"), pybind11::arg("param"), pybind11::arg("config"));

    // 绑定 radardsp_input_new_frame 函数
    m.def("radardsp_input_new_frame", &radardsp_input_new_frame, "Input a new radar frame", pybind11::arg("radar"), pybind11::arg("rdms"));
}

void bind_radar_processer(pybind11::module_ &m)
{
    // 绑定 radar_init_param_t 结构体
    bind_radar_init_param(m);

    // 绑定 radar 函数
    bind_radar_functions(m);
}

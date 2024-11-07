#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pyradar_fixed_radar_types.hh>

#include <stdexcept>

namespace py = pybind11;


void bind_measurements(pybind11::module_ &m)
{
    pybind11::class_<radar_measurements_t>(m, "measurements")
        .def(pybind11::init<>())
        .def_readwrite("distance", &radar_measurements_t::distance)
        .def_readwrite("velocity", &radar_measurements_t::velocity)
        .def_readwrite("azimuth", &radar_measurements_t::azimuth)
        .def_readwrite("amp", &radar_measurements_t::amp)
        .def_readwrite("snr", &radar_measurements_t::snr);
}


void bind_measurements_list(pybind11::module_ &m)
{
    pybind11::class_<radar_measurement_list_t>(m, "RadarMeasurementList")
        .def(pybind11::init<>()) // 默认构造函数
        .def_readwrite("num", &radar_measurement_list_t::num)
        .def_readwrite("capacity", &radar_measurement_list_t::capacity)
        // 使用 Python 列表访问 meas
        .def_property(
            "meas",
            [](radar_measurement_list_t &self) {
                // 创建并返回一个 Python 列表对象
                pybind11::list measurements;
                for (size_t i = 0; i < self.capacity; ++i) {
                    measurements.append(pybind11::cast(self.meas + i)); // 将 C 数组元素封装成 Python 对象
                }
                return measurements;
            },
            [](radar_measurement_list_t &self, const pybind11::list &measurements) {
                // 赋值并更新内部数据
                if (self.meas == nullptr) {
                    throw std::runtime_error("Invalid state: meas is nullptr");
                }
                if (measurements.size() > self.capacity) {
                    throw std::runtime_error("Invalid state: too many measurements");
                }
                self.num = measurements.size();
                for (size_t i = 0; i < self.num; ++i) {
                    // 将 Python 列表中的元素复制到 C 数组
                    self.meas[i] = pybind11::cast<radar_measurements_t>(measurements[i]);
                }
            }
        );
}

void bind_param(pybind11::module_ &m)
{
    pybind11::class_<radar_param_t>(m, "param")
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
        .def_readwrite("resVelocity", &radar_param_t::resVelocity);
}

void bind_radar_measurement_list_alloc(pybind11::module_ &m)
{
    m.def("radar_measurement_list_alloc", &radar_measurement_list_alloc, pybind11::return_value_policy::take_ownership, "Allocate a new RadarMeasurementList with the specified capacity.");
}

void bind_radar_types(pybind11::module_ &m)
{
    bind_measurements(m);
    bind_measurements_list(m);
    bind_param(m);
    bind_radar_measurement_list_alloc(m);
}

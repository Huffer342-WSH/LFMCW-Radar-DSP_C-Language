#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "module_wrapper.hh"
#include "radar_types.h"
#include "radar_processer.h"

namespace py = pybind11;



PYBIND11_MODULE(pyRadar, m)
{
    // 将 radar_param_t 结构体绑定到 Python 类 RadarParam
    py::class_<radar_param_t>(m, "RadarParam")
        .def(py::init<>())
        .def_readwrite("wavelength", &radar_param_t::wavelength)
        .def_readwrite("bandwidth", &radar_param_t::bandwidth)
        .def_readwrite("timeChrip", &radar_param_t::timeChrip)
        .def_readwrite("timeChripGap", &radar_param_t::timeChripGap)
        .def_readwrite("timeFrameGap", &radar_param_t::timeFrameGap)
        .def_readwrite("numChannel", &radar_param_t::numChannel)
        .def_readwrite("numPoint", &radar_param_t::numPoint)
        .def_readwrite("numRangeBin", &radar_param_t::numRangeBin)
        .def_readwrite("numChrip", &radar_param_t::numChrip)
        .def_readwrite("timeFrameVaild", &radar_param_t::timeFrameVaild)
        .def_readwrite("resRange", &radar_param_t::resRange)
        .def_readwrite("resVelocity", &radar_param_t::resVelocity);

    // 将 radar_config_t 结构体绑定到 Python 类 RadarConfig
    py::class_<radar_config_t>(m, "RadarConfig")
        .def(py::init<>())
        .def_readwrite("cfarGuardRange", &radar_config_t::cfarGuardRange)
        .def_readwrite("cfarGuardVelo", &radar_config_t::cfarGuardVelo)
        .def_readwrite("cfarTrainRange", &radar_config_t::cfarTrainRange)
        .def_readwrite("cfarTrainVelo", &radar_config_t::cfarTrainVelo)
        .def_readwrite("cfarThSNR", &radar_config_t::cfarThSNR)
        .def_readwrite("cfarThAmp", &radar_config_t::cfarThAmp);

    // 将 radar_basic_data_t 结构体绑定到 Python 类 RadarBasicData
    py::class_<radar_basic_data_t>(m, "RadarBasicData")
        .def(
            py::init<>(),
            "Default constructor initializes numChannel and numRangeBin to 0 and staticClutter to nullptr")
        .def_property(
            "staticClutter",
            [](radar_basic_data_t &self) -> py::array_t<std::complex<double> > {
                std::vector<size_t> shape = { self.param->numChannel, self.param->numRangeBin };
                return array_c2numpy<std::complex<double> >(self.staticClutter, shape);
            },
            [](radar_basic_data_t &self,
               py::array_t<std::complex<double>, py::array::c_style | py::array::forcecast> arr) {
                std::vector<size_t> shape = { self.param->numChannel, self.param->numRangeBin };
                array_numpy2c<std::complex<double> >(self.staticClutter, arr, shape);
            })
        .def_property(
            "ampSpec2D",
            [](radar_basic_data_t &self) -> py::array_t<double> {
                std::vector<size_t> shape = { self.param->numRangeBin, self.param->numChrip };
                return array_c2numpy<double>(self.ampSpec2D, shape);
            },
            [](radar_basic_data_t &self,
               py::array_t<double, py::array::c_style | py::array::forcecast> arr) {
                std::vector<size_t> shape = { self.param->numRangeBin, self.param->numChrip };
                array_numpy2c<double>(self.ampSpec2D, arr, shape);
            });


    // 将 radar_handle_t 结构体绑定到 Python 类 RadarHandle
    py::class_<radar_handle_t>(m, "RadarHandle")
        .def(py::init<>())
        .def_readwrite("cntFrame", &radar_handle_t::cntFrame)
        .def_readwrite("param", &radar_handle_t::param)
        .def_readwrite("config", &radar_handle_t::config)
        .def_readwrite("basic", &radar_handle_t::basic);



    // 将 radardsp_init 函数包装成 Python 可调用的函数
    m.def("radardsp_init", [](radar_handle_t &radar) -> int {
        radardsp_init(&radar); // 将 radar_handle_t 对象的引用传递给 radardsp_init
        return 0;
    });

    // 封装 radardsp_input_new_frame
    m.def("radardsp_input_new_frame",
          [](radar_handle_t &radar,
             py::array_t<std::complex<double>, py::array::c_style | py::array::forcecast> array)
              -> int {
              // 获取数组形状
              int shape0 = radar.param.numChannel;
              int shape1 = radar.param.numRangeBin;
              int shape2 = radar.param.numChrip;

              // 检查输入数组的形状是否匹配
              py::buffer_info buf_info = array.request();
              if (buf_info.ndim != 3 || buf_info.shape[0] != shape0 ||
                  buf_info.shape[1] != shape1 || buf_info.shape[2] != shape2) {
                  std::stringstream ss;
                  ss << "Input array shape does not match radar configuration. "
                     << "Expected shape (" << shape0 << ", " << shape1 << ", " << shape2 << "), "
                     << "but got shape (";
                  for (int i = 0; i < buf_info.ndim; ++i) {
                      ss << buf_info.shape[i];
                      if (i < buf_info.ndim - 1) {
                          ss << ", ";
                      }
                  }
                  ss << ").";
                  throw std::runtime_error(ss.str());
              }

              // 将数据指针传递给 C 函数
              void *data_ptr = buf_info.ptr;
              return radardsp_input_new_frame(&radar, data_ptr);
          });
}

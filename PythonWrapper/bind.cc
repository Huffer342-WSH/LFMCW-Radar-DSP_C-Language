#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
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
        .def_readwrite("numChannel", &radar_basic_data_t::numChannel)
        .def_readwrite("numRangeBin", &radar_basic_data_t::numRangeBin)
        .def_property(
            "staticClutter",
            [](radar_basic_data_t &self) -> py::array_t<std::complex<double> > {
                if (!self.staticClutter) {
                    // 如果 staticClutter 为空，返回空的 numpy 数组
                    return py::array_t<std::complex<double> >(
                        { 0, 0 },                             // Shape
                        { sizeof(std::complex<double>),       // Strides (step size in bytes)
                          sizeof(std::complex<double>) * 0 }, // Adjusted for your array size
                        nullptr // Data pointer (nullptr if you don't have one yet)
                    );
                }

                // 创建 numpy 数组，返回指向 staticClutter 的数据
                return py::array_t<std::complex<double> >(
                    { self.numChannel, self.numRangeBin }, // 形状
                    { sizeof(std::complex<double>) * self.numRangeBin,
                      sizeof(std::complex<double>) },                            // 步长
                    reinterpret_cast<std::complex<double> *>(self.staticClutter) // 数据指针
                );
            },
            [](radar_basic_data_t &self, py::array_t<std::complex<double> > arr) {
                py::buffer_info buf_info = arr.request();

                if (buf_info.ndim != 2 || buf_info.shape[0] != self.numChannel ||
                    buf_info.shape[1] != self.numRangeBin) {
                    throw std::runtime_error("Shape mismatch for staticClutter array");
                }

                // 如果 staticClutter 为空，分配内存
                if (!self.staticClutter) {
                    self.staticClutter = (double *)malloc(self.numChannel * self.numRangeBin *
                                                          sizeof(std::complex<double>));
                }

                // 直接将数据写入 staticClutter
                std::memcpy(self.staticClutter, buf_info.ptr, buf_info.size * buf_info.itemsize);
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
          [](radar_handle_t &radar, py::array_t<std::complex<double> > array) -> int {
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

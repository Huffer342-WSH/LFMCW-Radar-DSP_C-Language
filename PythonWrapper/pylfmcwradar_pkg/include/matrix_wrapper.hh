#ifndef _MODULE_WRAPPER_H_
#define _MODULE_WRAPPER_H_

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/detail/common.h>
#include <iostream>

namespace py = pybind11;

/**
 * 将C语言数组转换为 numpy 数组
 * @param data     C语言数组的指针
 * @param shape    C语言数组的形状
 * @return         numpy 数组
 */
template <typename T> pybind11::array_t<T> array_c2numpy(void *data, const std::vector<size_t> &shape)
{
    if (!data) {
        throw std::runtime_error("array is NULL");
        return pybind11::array_t<T>({ 0 });
    }
    py::capsule buffer_handle([]() { });

    return pybind11::array_t<T>(shape, static_cast<T *>(data), buffer_handle);
}


template <typename T> void array_numpy2c(void *data, pybind11::array arr, const std::vector<size_t> &shape)
{
    pybind11::buffer_info buf_info = arr.request();

    // 检查数组维度和形状是否匹配
    if (buf_info.ndim != shape.size()) {
        throw std::runtime_error("Dimension mismatch for array");
    }

    for (size_t i = 0; i < shape.size(); ++i) {
        if (buf_info.shape[i] != shape[i]) {
            throw std::runtime_error("Shape mismatch for array");
        }
    }

    // 检查数组的数据类型是否匹配模板类型 T
    if (!pybind11::isinstance<pybind11::array_t<T>>(arr)) {
        throw std::runtime_error("Dtype mismatch for array");
    }

    // 计算总大小
    size_t total_size = 1;
    for (size_t dim : shape) {
        total_size *= dim;
    }

    if (buf_info.size != total_size) {
        throw std::runtime_error("Size mismatch for array");
    }
    std::memcpy(data, buf_info.ptr, buf_info.size * buf_info.itemsize);
}

#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif
#endif /* _MODULE_WRAPPER_H_ */

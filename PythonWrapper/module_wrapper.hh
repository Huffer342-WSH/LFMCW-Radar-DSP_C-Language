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
template <typename T> py::array_t<T> array_c2numpy(void *data, const std::vector<size_t> &shape)
{
    if (!data) {
        throw std::runtime_error("array is NULL");
        return py::array_t<T>({ 0 });
    }
    // 计算步长
    std::vector<size_t> strides(shape.size());
    size_t stride = sizeof(T);
    for (int i = static_cast<int>(shape.size()) - 1; i >= 0; --i) {
        strides[i] = stride;
        stride *= shape[i];
    }
    return py::array_t<T>(shape, strides, static_cast<T *>(data));
}



template <typename T>
void array_numpy2c(void *data, py::array arr, const std::vector<size_t> &shape)
{
    py::buffer_info buf_info = arr.request();

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
    if (!py::isinstance<py::array_t<T> >(arr)) {
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

    // 如果数据为空，则分配新的内存
    if (!data) {
        throw std::runtime_error("Data pointer is NULL");
    }

    // 检查内存是否是连续的
    bool is_contiguous = true;
    long long expected_stride = buf_info.itemsize;
    for (int i = static_cast<int>(buf_info.ndim) - 1; i >= 0; --i) {
        if (buf_info.strides[i] != expected_stride) {
            is_contiguous = false;
            break;
        }
        expected_stride *= buf_info.shape[i];
    }

    // 如果是连续内存，使用memcpy进行快速拷贝
    if (is_contiguous) {
        std::memcpy(data, buf_info.ptr, buf_info.size * buf_info.itemsize);
    } else {
        throw std::runtime_error("Memory is not contiguous");
        // 否则，进行逐元素拷贝
        T *dst = static_cast<T *>(data);
        T *src = static_cast<T *>(buf_info.ptr);
        std::vector<size_t> indices(buf_info.ndim, 0);

        for (size_t i = 0; i < total_size; ++i) {
            // 计算在原数据中的偏移量
            long long src_offset = 0;
            for (int j = 0; j < buf_info.ndim; ++j) {
                src_offset += indices[j] * buf_info.strides[j] / buf_info.itemsize;
            }

            // 拷贝数据到目标内存
            dst[i] = src[src_offset];

            // 更新indices（多维数组下标遍历）
            for (int j = static_cast<int>(buf_info.ndim) - 1; j >= 0; --j) {
                if (++indices[j] < static_cast<size_t>(buf_info.shape[j])) {
                    break;
                }
                indices[j] = 0;
            }
        }
    }
}

#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif
#endif /* _MODULE_WRAPPER_H_ */

#pragma once
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

// 模板函数：将 C 数组封装成 Python list
template <typename T> py::list convert_cstyle_struct_array_to_pylist(T *data, size_t num)
{
    py::list list;
    for (size_t i = 0; i < num; ++i) {
        list.append(pybind11::cast(data[i])); // 将 C 数组元素封装成 Python 对象
    }
    return list;
}

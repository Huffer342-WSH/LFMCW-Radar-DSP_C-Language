
#include "pyradar_fixed_radar_matrix.hh"

#include "matrix_wrapper.hh"

#include "radar_matrix.h"


void bind_matrix3d_complex_int16(pybind11::module_ &m)
{
    pybind11::class_<matrix3d_complex_int16_t>(m, "matrix3d_complex_int16", py::buffer_protocol())
        .def(pybind11::init<>(), "Use matrix3d_complex_int16_alloc() to create a new matrix3d_complex_int16")
        .def_readwrite("size0", &matrix3d_complex_int16_t::size0)
        .def_readwrite("size1", &matrix3d_complex_int16_t::size1)
        .def_readwrite("size2", &matrix3d_complex_int16_t::size2)
        .def_readwrite("tda1", &matrix3d_complex_int16_t::tda1)
        .def_readwrite("tda2", &matrix3d_complex_int16_t::tda2)
        .def_readwrite("owner", &matrix3d_complex_int16_t::owner)
        .def_property(
            "data",
            [](matrix3d_complex_int16_t &self) {
                std::vector<size_t> shape = { self.size0, self.size1, self.size2, 2 };
                return array_c2numpy<int16_t>(self.data, shape);
            },
            [](matrix3d_complex_int16_t &self, py::array arr) {
                std::vector<size_t> shape = { self.size0, self.size1, self.size2, 2 };
                array_numpy2c<int16_t>(self.data, arr, shape);
            },
            "matrix3d_complex_int16_t is a 3D matrix of complex int16_t, but numpy does not support complex int16_t dtype, so we add a new dimension for real and imag"
        );
}

void bind_matrix2d_complex_int32(pybind11::module_ &m)
{
    pybind11::class_<matrix2d_complex_int32_t>(m, "matrix2d_complex_int32", py::buffer_protocol())
        .def(pybind11::init<>(), "Use matrix2d_complex_int32_alloc() to create a new matrix2d_complex_int32")
        .def_readwrite("size0", &matrix2d_complex_int32_t::size0)
        .def_readwrite("size1", &matrix2d_complex_int32_t::size1)
        .def_readwrite("tda1", &matrix2d_complex_int32_t::tda1)
        .def_readwrite("owner", &matrix2d_complex_int32_t::owner)
        .def_property(
            "data",
            [](matrix2d_complex_int32_t &self) {
                std::vector<size_t> shape = { self.size0, self.size1, 2 }; // 2 for real and imaginary parts
                return array_c2numpy<int32_t>(self.data, shape);           // 转换为NumPy数组
            },
            [](matrix2d_complex_int32_t &self, py::array arr) {
                std::vector<size_t> shape = { self.size0, self.size1, 2 };
                array_numpy2c<int32_t>(self.data, arr, shape); // 从NumPy数组更新数据
            },
            "matrix2d_complex_int32_t is a 2D matrix of complex int32_t values, represented by two parts (real, imaginary)"
        );
}


void bind_matrix2d_complex_int16(pybind11::module_ &m)
{
    pybind11::class_<matrix2d_complex_int16_t>(m, "matrix2d_complex_int16", py::buffer_protocol())
        .def(pybind11::init<>(), "Use matrix2d_complex_int16_alloc() to create a new matrix2d_complex_int16")
        .def_readwrite("size0", &matrix2d_complex_int16_t::size0)
        .def_readwrite("size1", &matrix2d_complex_int16_t::size1)
        .def_readwrite("tda1", &matrix2d_complex_int16_t::tda1)
        .def_readwrite("owner", &matrix2d_complex_int16_t::owner)
        .def_property(
            "data",
            [](matrix2d_complex_int16_t &self) {
                std::vector<size_t> shape = { self.size0, self.size1, 2 }; // 2 for real and imaginary parts
                return array_c2numpy<int16_t>(self.data, shape);
            },
            [](matrix2d_complex_int16_t &self, py::array arr) {
                std::vector<size_t> shape = { self.size0, self.size1, 2 };
                array_numpy2c<int16_t>(self.data, arr, shape);
            },
            "matrix2d_complex_int16_t is a 2D matrix of complex int16_t, represented by two dimensions for real and imaginary parts"
        );
}

void bind_matrix2d_int32(pybind11::module_ &m)
{
    pybind11::class_<matrix2d_int32_t>(m, "matrix2d_int32", py::buffer_protocol())
        .def(pybind11::init<>(), "Use matrix2d_int32_alloc() to create a new matrix2d_int32")
        .def_readwrite("size0", &matrix2d_int32_t::size0)
        .def_readwrite("size1", &matrix2d_int32_t::size1)
        .def_readwrite("tda1", &matrix2d_int32_t::tda1)
        .def_readwrite("owner", &matrix2d_int32_t::owner)
        .def_property(
            "data",
            [](matrix2d_int32_t &self) {
                std::vector<size_t> shape = { self.size0, self.size1 };
                return array_c2numpy<int32_t>(self.data, shape); // 传递二维数据
            },
            [](matrix2d_int32_t &self, py::array arr) {
                std::vector<size_t> shape = { self.size0, self.size1 };
                array_numpy2c<int32_t>(self.data, arr, shape); // 更新C++结构体中的数据
            },
            "matrix2d_int32_t is a 2D matrix of int32_t values"
        );
}


void bind_matrix2d_int16(pybind11::module_ &m)
{
    pybind11::class_<matrix2d_int16_t>(m, "matrix2d_int16", py::buffer_protocol())
        .def(pybind11::init<>(), "Use matrix2d_int16_alloc() to create a new matrix2d_int16")
        .def_readwrite("size0", &matrix2d_int16_t::size0)
        .def_readwrite("size1", &matrix2d_int16_t::size1)
        .def_readwrite("tda1", &matrix2d_int16_t::tda1)
        .def_readwrite("owner", &matrix2d_int16_t::owner)
        .def_property(
            "data",
            [](matrix2d_int16_t &self) {
                std::vector<size_t> shape = { self.size0, self.size1 };
                return array_c2numpy<int16_t>(self.data, shape); // 仅返回二维数组
            },
            [](matrix2d_int16_t &self, py::array arr) {
                std::vector<size_t> shape = { self.size0, self.size1 };
                array_numpy2c<int16_t>(self.data, arr, shape); // 从NumPy数组更新数据
            },
            "matrix2d_int16_t is a 2D matrix of int16_t values"
        );
}


void bind_radar_matrix3d_complex_int16_alloc(pybind11::module_ &m)
{
    m.def("matrix3d_complex_int16_alloc", &radar_matrix3d_complex_int16_alloc, pybind11::return_value_policy::take_ownership, "Allocate a new matrix3d_complex_int16");
}


void bind_radar_matrix2d_complex_int32_alloc(pybind11::module_ &m)
{
    m.def("matrix2d_complex_int32_alloc", &radar_matrix2d_complex_int32_alloc, pybind11::return_value_policy::take_ownership, "Allocate a new matrix2d_complex_int32");
}

void bind_radar_matrix2d_complex_int16_alloc(pybind11::module_ &m)
{
    m.def("matrix2d_complex_int16_alloc", &radar_matrix2d_complex_int16_alloc, pybind11::return_value_policy::take_ownership, "Allocate a new matrix2d_complex_int16");
}

void bind_radar_matrix2d_int32_alloc(pybind11::module_ &m)
{
    m.def("matrix2d_int32_alloc", &radar_matrix2d_int32_alloc, pybind11::return_value_policy::take_ownership, "Allocate a new matrix2d_int32");
}

void bind_radar_matrix2d_int16_alloc(pybind11::module_ &m)
{
    m.def("matrix2d_int16_alloc", &radar_matrix2d_int16_alloc, pybind11::return_value_policy::take_ownership, "Allocate a new matrix2d_int16");
}


void bind_radar_matrix(pybind11::module_ &m)
{
    bind_matrix3d_complex_int16(m);
    bind_matrix2d_complex_int32(m);
    bind_matrix2d_complex_int16(m);
    bind_matrix2d_int32(m);
    bind_matrix2d_int16(m);

    bind_radar_matrix3d_complex_int16_alloc(m);
    bind_radar_matrix2d_complex_int32_alloc(m);
    bind_radar_matrix2d_complex_int16_alloc(m);
    bind_radar_matrix2d_int32_alloc(m);
    bind_radar_matrix2d_int16_alloc(m);
}

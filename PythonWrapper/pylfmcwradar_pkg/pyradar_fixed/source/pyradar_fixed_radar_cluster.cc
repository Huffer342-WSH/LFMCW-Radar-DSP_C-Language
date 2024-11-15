#include "pyradar_fixed_radar_cluster.hh"
#include "radar_cluster.h"

#include <matrix_wrapper.hh>
#include <list_wrapper.hh>


void bind_dbscan_neighbors(pybind11::module_ &m)
{

    pybind11::class_<dbscan_neighbors_t>(m, "dbscan_neighbors")
        .def(pybind11::init<>()) //
        .def_readwrite("n", &dbscan_neighbors_t::n);
}


void bind_radar_cluster_dbscan(pybind11::module_ &m)
{
    m.def(
        "radar_cluster_dbscan",
        [](radar_measurement_list_fixed_t *meas, int32_t wr, int32_t wv, int32_t eps, size_t min_samples) -> py::array_t<ssize_t> {
            // 获取输入的测量数据数组

            size_t *labels = new size_t[meas->num];


            // 调用原始的 C++ 函数
            radar_cluster_dbscan(labels, meas, wr, wv, eps, min_samples);

            // 返回一个 NumPy 数组
            return py::array_t<ssize_t>({ meas->num },      // 形状
                                        { sizeof(size_t) }, // 步长
                                        (ssize_t *)labels); // 数据指针
        },
        py::arg("meas_array"), py::arg("wr"), py::arg("wv"), py::arg("eps"), py::arg("min_samples"), "Cluster radar measurements using DBSCAN.");
}


void bind_radar_cluster(pybind11::module_ &m)
{
    bind_dbscan_neighbors(m);
    bind_radar_cluster_dbscan(m);
}

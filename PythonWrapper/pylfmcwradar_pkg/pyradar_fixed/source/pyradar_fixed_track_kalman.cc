#include "pyradar_fixed_track_kalman.hh"
#include "track_kalman.hh"
#include <pybind11/eigen.h>


namespace py = pybind11;

void bind_tracker_kalmen(pybind11::module_ &m)
{
    py::class_<GaussianState>(m, "GaussianState")
        .def(py::init<>())
        .def(py::init<const Vector4r &, const Matrix44r &, uint32_t>())
        .def(py::init<rd_float_t *, rd_float_t *, uint32_t>())
        .def_readwrite("state_vector", &GaussianState::state_vector)
        .def_readwrite("covar", &GaussianState::covar)
        .def_readwrite("timestamp_ms", &GaussianState::timestamp_ms);
}

#include "pyradar_fixed_track_target.hh"
#include "track_target.hh"

namespace py = pybind11;


void _bind_c_targtes(pybind11::module_ &m)
{
}

void bind_track_target(pybind11::module_ &m)
{
    py::class_<LifeCycle>(m, "LifeCycle")
        .def(py::init<int32_t>())
        .def_readwrite("score", &LifeCycle::score)
        .def_readwrite("unassociated_time", &LifeCycle::unassociated_time)
        .def_readwrite("deducted_score", &LifeCycle::deducted_score);


    py::class_<TrackedTarget>(m, "TrackedTarget")
        .def(py::init<uint32_t, GaussianState &>())
        .def_readwrite("uuid", &TrackedTarget::uuid)
        .def_readwrite("state", &TrackedTarget::state)
        .def_readwrite("life_cycle", &TrackedTarget::life_cycle);
}

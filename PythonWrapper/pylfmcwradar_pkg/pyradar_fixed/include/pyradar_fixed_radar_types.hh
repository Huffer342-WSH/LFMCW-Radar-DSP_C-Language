#pragma once
#include <pybind11/pybind11.h>

#include "matrix_wrapper.hh"

#include "radar_types.h"
#include "radar_processer.h"

void bind_measurements(pybind11::module_ &m);
void bind_measurements_list(pybind11::module_ &m);
void bind_param(pybind11::module_ &m);

void bind_radar_types(pybind11::module_ &m);

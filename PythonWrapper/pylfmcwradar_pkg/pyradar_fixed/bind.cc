#include "pyradar_fixed_radar_cfar.hh"
#include "pyradar_fixed_radar_types.hh"

PYBIND11_MODULE(pyradar_fixed, m)
{
    bind_cfar(m);
    bind_radar_types(m);
}

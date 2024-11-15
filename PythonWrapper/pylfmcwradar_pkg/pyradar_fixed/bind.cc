#include "pyradar_fixed_radar_matrix.hh"
#include "pyradar_fixed_radar_cfar.hh"
#include "pyradar_fixed_radar_processer.hh"
#include "pyradar_fixed_radar_types.hh"
#include "pyradar_fixed_radar_cluster.hh"


PYBIND11_MODULE(pyradar_fixed, m)
{
    // 注意顺序
    bind_radar_matrix(m);
    bind_cfar(m);
    bind_radar_types(m);
    bind_radar_cluster(m);
    bind_radar_processer(m);
}

cmake_minimum_required(VERSION 3.12)

add_library(RadarDP_SigProc_fixed_objs OBJECT)
add_library(RadarDP_SigProc_fixed STATIC)

target_link_libraries(RadarDP_SigProc_fixed_objs PUBLIC
    CMSISDSP
    RadarDP_Common
)

target_sources(RadarDP_SigProc_fixed_objs PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/radar_cfar.c
    ${CMAKE_CURRENT_SOURCE_DIR}/radar_cluster.c
    ${CMAKE_CURRENT_SOURCE_DIR}/radar_dis_velo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/radar_doa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/radar_mag.c
    ${CMAKE_CURRENT_SOURCE_DIR}/radar_math.c
    ${CMAKE_CURRENT_SOURCE_DIR}/radar_matrix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/radar_measurement.c
    ${CMAKE_CURRENT_SOURCE_DIR}/radar_micromotion.c
    ${CMAKE_CURRENT_SOURCE_DIR}/radar_types.c
)

target_link_libraries(RadarDP_SigProc_fixed PUBLIC
    RadarDP_SigProc_fixed_objs
)

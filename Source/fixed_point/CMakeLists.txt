cmake_minimum_required(VERSION 3.15)

project(LFMCW-Radar-Processer-FixedPointEdition)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)


add_library(lfmcwradar_fixed STATIC)

target_link_libraries(lfmcwradar_fixed PUBLIC
    CMSISDSP
    stc
)

target_include_directories(lfmcwradar_fixed PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_include_directories(lfmcwradar_fixed PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/PrivateInclude
)

target_sources(lfmcwradar_fixed PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/radar_assert.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/radar_cfar.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/radar_cluster.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/radar_dis_velo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/radar_doa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/radar_error.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/radar_mag.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/radar_math.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/radar_matrix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/radar_measurement.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/radar_micromotion.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/radar_processer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/radar_types.c
)

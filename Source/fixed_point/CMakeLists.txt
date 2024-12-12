cmake_minimum_required(VERSION 3.15)

project(LFMCW-Radar-Processer-FixedPointEdition)

# 设置默认路径为当前源目录下的 "Portable" 文件夹
set(RADAR_CONFIG_FILE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Portable" CACHE STRING "Path to RadarConfig.h")

if(RADAR_CONFIG_FILE_DIRECTORY STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}/Portable")
    message(WARNING "Default RADAR_CONFIG_FILE_DIRECTORY is used")
else()
    if(EXISTS "${RADAR_CONFIG_FILE_DIRECTORY}/RadarConfig.h")
        message(STATUS "Using RADAR_CONFIG_FILE_DIRECTORY: ${RADAR_CONFIG_FILE_DIRECTORY}")
    else()
        message(FATAL_ERROR "The specified RADAR_CONFIG_FILE_DIRECTORY (${RADAR_CONFIG_FILE_DIRECTORY}) does not contain RadarConfig.h")
    endif()
endif()





add_library(lfmcwradar_fixed STATIC)

target_link_libraries(lfmcwradar_fixed PUBLIC
    CMSISDSP
    stc
)

target_include_directories(lfmcwradar_fixed PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
    ${RADAR_CONFIG_FILE_DIRECTORY}
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

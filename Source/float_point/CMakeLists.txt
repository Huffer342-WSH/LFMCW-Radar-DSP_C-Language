cmake_minimum_required(VERSION 3.10)
project(LFMCW-Radar-Processer-FloatPointEdition)

add_library(lfmcwradar_float STATIC)

target_include_directories(lfmcwradar_float PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_include_directories(lfmcwradar_float PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/PrivateInclude
)

target_sources(lfmcwradar_float PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/radar_processer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/spec_clustering.c
)

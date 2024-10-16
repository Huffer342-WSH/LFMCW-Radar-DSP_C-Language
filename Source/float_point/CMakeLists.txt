cmake_minimum_required(VERSION 3.10)
project(LFMCW-Radar-Processer-FloatPointEdition)

# 指定FFTW的安装路径
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/ThirdParty/FFTW")

add_library(lfmcwradar_float STATIC)

target_include_directories(lfmcwradar_float PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_include_directories(lfmcwradar_float PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/PrivateInclude
)

target_sources(lfmcwradar_float PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/radar_processer.c
)

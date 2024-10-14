cmake_minimum_required(VERSION 3.10)
project(LFMCW_Radar_DSP_FloatPoint)

# 指定FFTW的安装路径
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/ThirdParty/FFTW")

add_library(LFMCW_Radar_DSP_FloatPoint STATIC)

target_include_directories(LFMCW_Radar_DSP_FloatPoint PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(LFMCW_Radar_DSP_FloatPoint PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/PrivateInclude
)

target_sources(LFMCW_Radar_DSP_FloatPoint PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/radar_processer.c
)

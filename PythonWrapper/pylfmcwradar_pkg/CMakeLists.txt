# Windwos平台的Python模块需要使用msvc构建，经过测试w64devkit发行的mingw64-gcc也可以编译模块并正常工作

cmake_minimum_required(VERSION 3.15)

set(PUBLIC_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(pyradar_fixed)

add_subdirectory(pyradar_float)

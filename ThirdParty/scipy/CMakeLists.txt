cmake_minimum_required(VERSION 3.15)

project(scipy C CXX)

add_library(scipy STATIC)

target_sources(scipy PRIVATE
    rectangular_lsap.cc
)

target_include_directories(scipy PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 判断是否是 ARM 平台，并且是 ARM Clang 编译器

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "ARMClang")
    target_compile_options(scipy PRIVATE -ffp-mode=full)
endif()

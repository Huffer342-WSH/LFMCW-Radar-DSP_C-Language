cmake_minimum_required(VERSION 3.15)

project(scipy C CXX)

add_library(scipy STATIC)

target_sources(scipy PRIVATE
    rectangular_lsap.cc
)

target_include_directories(scipy PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

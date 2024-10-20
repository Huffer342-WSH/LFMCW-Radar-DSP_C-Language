cmake_minimum_required(VERSION 3.10)
project(test-cfar2d-floatpoint-edition)

add_executable(test-cfar2d-floatpoint-edition main.c)

target_link_libraries(test-cfar2d-floatpoint-edition
    lfmcwradar_float
)

target_compile_options(test-cfar2d-floatpoint-edition PRIVATE
    -O0
)

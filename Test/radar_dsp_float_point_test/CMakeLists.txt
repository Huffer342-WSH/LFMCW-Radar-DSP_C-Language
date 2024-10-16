cmake_minimum_required(VERSION 3.10)
project(test-lfmcwradar-floatpoint-edition)

add_executable(test-lfmcwradar-floatpoint-edition main.c)

target_link_libraries(test-lfmcwradar-floatpoint-edition 
lfmcwradar_float
fftw3
CMSISDSP
)

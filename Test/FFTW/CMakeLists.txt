cmake_minimum_required(VERSION 3.10)
project(Test_FFTW)

# 链接FFTW库
add_executable(test-fftw main.c)
target_link_libraries(test-fftw fftw3)

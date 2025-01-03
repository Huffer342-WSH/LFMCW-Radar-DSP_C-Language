cmake_minimum_required(VERSION 3.10)

if(ENABLE_TEST_FFTW)
    add_executable(test-fftw main.c)
    target_link_libraries(test-fftw fftw3)
endif()

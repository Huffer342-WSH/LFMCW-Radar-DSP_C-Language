include(GoogleTest)

add_compile_options(${ADDRESSSANITIZER_FLAGS})
link_libraries(${ADDRESSSANITIZER_LIBRARIES})

# FFTW测试
if(ENABLE_TEST_FFTW)
    add_subdirectory(FFTW)
endif()

# CMSIS-DSP测试
if(ENABLE_TEST_ARM_CFFT_Q15)
    add_subdirectory(CMSIS-DSP)
endif()

# STC测试
add_subdirectory(STC)

# 雷达信号处理库测试
add_subdirectory(Radar_Fixed)


add_subdirectory(Python)

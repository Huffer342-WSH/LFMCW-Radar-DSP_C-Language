include(GoogleTest)

if(ADDRESSSANITIZER_FLAGS)
add_compile_options(${ADDRESSSANITIZER_FLAGS})
link_libraries(${ADDRESSSANITIZER_LIBRARIES})
endif()

# FFTW测试
add_subdirectory(FFTW)

# CMSIS-DSP测试
add_subdirectory(CMSIS-DSP)

# Eigen测试
add_subdirectory(Eigen)

add_subdirectory(algorithm)

add_subdirectory(lib)

# 雷达信号处理库测试
add_subdirectory(Radar_Fixed)

if(BUILD_PythonWrapper)
    add_subdirectory(Python)
endif()

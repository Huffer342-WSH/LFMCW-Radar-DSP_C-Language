include(GoogleTest)

add_compile_options(${ADDRESSSANITIZER_FLAGS})
link_libraries(${ADDRESSSANITIZER_LIBRARIES})

# FFTW测试
add_subdirectory(FFTW)

# CMSIS-DSP测试
add_subdirectory(CMSIS-DSP)

# STC测试
add_subdirectory(STC)

# Eigen测试
add_subdirectory(Eigen)

add_subdirectory(algorithm)

# 雷达信号处理库测试
add_subdirectory(Radar_Fixed)

add_subdirectory(Python)

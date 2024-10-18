# 第三方库测试
add_subdirectory(FFTW EXCLUDE_FROM_ALL)
add_subdirectory(CMSIS-DSP EXCLUDE_FROM_ALL)

# 雷达信号处理库测试
add_subdirectory(radar_dsp_float_point_test)

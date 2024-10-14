cmake_minimum_required(VERSION 3.10)
project(RadarDSP_FixedPoint_Test)

add_executable(RadarDSP_FixedPoint_Test main.c)

target_link_libraries(RadarDSP_FixedPoint_Test LFMCW_Radar_DSP_FloatPoint)

# 链接FFTW库
target_link_libraries(RadarDSP_FixedPoint_Test FFTW)

add_custom_command(TARGET RadarDSP_FixedPoint_Test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${FFTW_LIBRARIES}
    $<TARGET_FILE_DIR:FFTW_TEST>)

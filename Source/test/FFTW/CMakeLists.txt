cmake_minimum_required(VERSION 3.10)
project(FFTW_TEST)

# 指定FFTW的安装路径
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/ThirdParty/FFTW")

# 使用find_package查找FFTW
# find_package(FFTW CONFIG REQUIRED)
message(STATUS "Found FFTW: ${FFTW_INCLUDE_DIR} ${FFTW_LIBRARIES}")

# 链接FFTW库
add_executable(FFTW_TEST main.c)
target_link_libraries(FFTW_TEST FFTW)

add_custom_command(TARGET FFTW_TEST POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${FFTW_LIBRARIES}
    $<TARGET_FILE_DIR:FFTW_TEST>)

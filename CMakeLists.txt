cmake_minimum_required(VERSION 3.10)
project(LFMCWRadarProcesser)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/ThirdParty/FFTW")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

find_package(FFTW CONFIG REQUIRED)

link_libraries(m)

# 编译依赖库
add_subdirectory(ThirdParty)

# 编译应用
add_subdirectory(Source)

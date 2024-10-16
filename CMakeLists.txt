cmake_minimum_required(VERSION 3.10)
project(LFMCWRadarProcesser)

# 设置 C++ 标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# 设置输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/.output/${CMAKE_BUILD_TYPE}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/.output/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/.output/${CMAKE_BUILD_TYPE}/)

# 添加全局依赖库
link_libraries(m)

# 编译依赖库
add_subdirectory(ThirdParty)

# 编译雷达信号处理库
add_subdirectory(Source)

# 编译测试
add_subdirectory(Test)

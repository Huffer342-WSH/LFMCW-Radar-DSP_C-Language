cmake_minimum_required(VERSION 3.15)

project(LFMCWRadarProcesser)

enable_testing()

# 设置 C++ 标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# 设置输出路径
set(TATGET_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/.output/${CMAKE_BUILD_TYPE})
set(EXECUTABLE_OUTPUT_PATH ${TATGET_OUTPUT_PATH}/bin)
set(LIBRARY_OUTPUT_PATH ${TATGET_OUTPUT_PATH}/lib)
set(CMAKE_INSTALL_PREFIX ${TATGET_OUTPUT_PATH}/)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

# 设置虚拟环境的 Python 路径（假设虚拟环境路径为 venv）
include(configPython3)

# 设置 MSVC 编译选项
include(msvc_compile_options)

include(configTest.cmake)

# 添加全局依赖库
# link_libraries(m)

# 编译依赖库
add_subdirectory(ThirdParty)

# 编译雷达信号处理库
add_subdirectory(Source)

# 构建C语言测试
add_subdirectory(Test)

# 构建Python模块
add_subdirectory(PythonWrapper)

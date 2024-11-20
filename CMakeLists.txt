cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0123 NEW)
project(LFMCWRadarProcesser)

option(CROSS_COMPILE "Cross compile" OFF)

# 设置 C++ 标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

# 设置输出路径
set(TATGET_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/.output/${CMAKE_BUILD_TYPE})
set(EXECUTABLE_OUTPUT_PATH ${TATGET_OUTPUT_PATH}/bin)
set(LIBRARY_OUTPUT_PATH ${TATGET_OUTPUT_PATH}/lib)
set(CMAKE_INSTALL_PREFIX ${TATGET_OUTPUT_PATH}/)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

# ==============================================================================
# 交叉编译
# ==============================================================================
if(CROSS_COMPILE)
    include(configCrossCompile)
    return()
endif()

# ==============================================================================
# 本地编译
# ==============================================================================
enable_testing()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(configPython) # 设置虚拟环境的 Python 路径（假设虚拟环境路径为 venv）

include(configAddressSanitizer) # 包含 AddressSanitizer 配置文件

include(configTest) # 配置测试

include(msvc_compile_options) # 设置 MSVC 编译选项

# 编译依赖库
add_subdirectory(ThirdParty)

# 编译雷达信号处理库
add_subdirectory(Source)

# 构建Python模块
add_subdirectory(PythonWrapper)

# 构建测试
add_subdirectory(Test)


add_subdirectory(Doc)

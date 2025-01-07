cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0123 NEW)
project(LFMCWRadarProcesser VERSION 0.2.0
    DESCRIPTION "适用于LFMCW雷达的数据处理算法库，最终希望提供一个可移植的C语言库，输入2D-FFT后的雷达中频信号，输出目标跟踪结果"
)

# 可修改选项
option(CROSS_COMPILE "Cross compile" OFF)

# 设置 C++ 标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS ON)

# 设置输出路径
set(TATGET_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/.output/${CMAKE_BUILD_TYPE})

set(EXECUTABLE_OUTPUT_PATH ${TATGET_OUTPUT_PATH}/bin)
set(LIBRARY_OUTPUT_PATH ${TATGET_OUTPUT_PATH}/lib)
set(CMAKE_INSTALL_PREFIX ${TATGET_OUTPUT_PATH}/)

# 添加 CMake 模块
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

include(configGitInfo)

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

# 设置虚拟环境的 Python 路径（假设虚拟环境路径为 venv）
include(configPython)

# 包含 AddressSanitizer 配置文件
include(configAddressSanitizer)

# 设置 MSVC 编译选项
include(msvc_compile_options)

# 配置测试
include(configTest) # 可修改

# 编译依赖库
add_subdirectory(ThirdParty)

# 编译雷达信号处理库
add_subdirectory(Source)

# 构建Python模块
add_subdirectory(PythonWrapper)

# 构建测试
add_subdirectory(Test)

# 构建文档
add_subdirectory(Doc)

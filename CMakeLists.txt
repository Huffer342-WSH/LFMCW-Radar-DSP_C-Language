cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0123 NEW)
project(LFMCWRadarProcesser VERSION 0.2.0
    DESCRIPTION "适用于LFMCW雷达的数据处理算法库，最终希望提供一个可移植的C语言库，输入2D-FFT后的雷达中频信号，输出目标跟踪结果"
)

# 可修改选项
option(CROSS_COMPILE "使能交叉编译" OFF)
option(BUILD_DOCUMENT "生成文档" OFF)

if(CROSS_COMPILE)
    option(BUILD_TEST "编译测试" OFF)
    option(BUILD_PythonWrapper "编译Python封装" OFF)
else()
    option(BUILD_TEST "编译测试" ON)
    option(BUILD_PythonWrapper "编译Python封装" ON)
endif()

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
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake")

include(configGitInfo)

# 包含 AddressSanitizer 配置文件
include(configAddressSanitizer) # 测试中使用了AddressSanitizer

# 设置 MSVC 编译选项
include(msvc_compile_options) # 解决CMake使用MSVC的Bug

# 编译依赖库
add_subdirectory(ThirdParty)

# 编译雷达信号处理库
add_subdirectory(Source)

if(BUILD_PythonWrapper)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    # 设置虚拟环境的 Python 路径（假设虚拟环境路径为 venv）
    include(configPython)

    add_subdirectory(ThirdParty/pybind11-2.13.6)

    # 构建Python模块
    add_subdirectory(PythonWrapper)
endif()

if(BUILD_TEST)
    enable_testing()

    # 配置测试
    include(configTest) # 可修改

    add_subdirectory(ThirdParty/googletest-1.15.2)

    # 构建测试
    add_subdirectory(Test)
endif()

# 构建文档
if(BUILD_DOCUMENT)
add_subdirectory(Doc)
endif()

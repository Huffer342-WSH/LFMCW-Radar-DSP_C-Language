cmake_minimum_required(VERSION 3.15)

# 设置生成的python包名称
set(PYTHON_PACKAGE_NAME pylfmcwradar)

# 检查并设置包生成路径
if(NOT DEFINED PYTHON_PACKAGE_OUTPUT_DIR)
    if(DEFINED TATGET_OUTPUT_PATH)
        set(PYTHON_PACKAGE_OUTPUT_DIR ${TATGET_OUTPUT_PATH}/python/${PYTHON_PACKAGE_NAME})
    else()
        message(FATAL_ERROR "TATGET_OUTPUT_PATH is not set")
    endif()
endif()

# 检查并设置包安装目录
if(NOT DEFINED PYTHON_PACKAGE_INATSLL_DIR)
    if(DEFINED Python_SITEARCH)
        set(PYTHON_PACKAGE_INATSLL_DIR "${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}")
    else()
        message(FATAL_ERROR "Python_SITEARCH is not set, use find_package(Python) before")
    endif()
endif()

# 添加源文件
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/pylfmcwradar_pkg)

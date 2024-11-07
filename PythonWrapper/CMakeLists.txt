cmake_minimum_required(VERSION 3.15)

project(pythonwrapper VERSION 0.1)

# 假设 Python3_EXECUTABLE 已经定义（例如通过 find_package 查找到）
if(Python3_EXECUTABLE)
    # 尝试运行 Python3_EXECUTABLE --version 来检查是否是有效的 Python 可执行文件
    execute_process(
        COMMAND ${Python3_EXECUTABLE} --version
        RESULT_VARIABLE PYTHON_EXEC_CHECK
        OUTPUT_VARIABLE PYTHON_VERSION_OUTPUT
        ERROR_VARIABLE PYTHON_VERSION_ERROR
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
    )

    if(PYTHON_EXEC_CHECK EQUAL 0)
        message(STATUS "Python executable found: ${Python3_EXECUTABLE}")
        message(STATUS "Python version output: ${PYTHON_VERSION_OUTPUT}")
    else()
        message(WARNING "The specified Python3_EXECUTABLE (${Python3_EXECUTABLE}) is not a valid Python executable.")
    endif()
else()
    message(FATAL_ERROR "Python3_EXECUTABLE is not set.")
endif()

# 设置生成的python包名称
set(PYTHON_PACKAGE_NAME pylfmcwradar)

# 设置包生成路径
set(PYTHON_PACKAGE_OUTPUT_DIR ${TATGET_OUTPUT_PATH}/python/${PYTHON_PACKAGE_NAME})

# 设置包安装目录
set(PYTHON_PACKAGE_INATSLL_DIR "${CMAKE_SOURCE_DIR}/.venv/Lib/site-packages/${PYTHON_PACKAGE_NAME}")

# 添加源文件
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/pylfmcwradar_pkg)

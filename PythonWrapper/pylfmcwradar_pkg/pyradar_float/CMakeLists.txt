cmake_minimum_required(VERSION 3.15)

# 项目名称和语言
project(pyradar_float)

pybind11_add_module(pyradar_float
    pyradar_float_processer.cc
)

target_include_directories(pyradar_float PRIVATE
    ${PUBLIC_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(pyradar_float PRIVATE
    lfmcwradar_float
)

set_target_properties(pyradar_float
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${PYTHON_PACKAGE_OUTPUT_DIR}"
)

# 生成头文件
add_custom_command(TARGET pyradar_float POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=${PYTHON_PACKAGE_OUTPUT_DIR}" ${Python3_EXECUTABLE} -m pybind11_stubgen pyradar_float --output-dir ${PYTHON_PACKAGE_OUTPUT_DIR}
    DEPENDS pyradar_float
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating .pyi file for pyRadar"
    VERBATIM
)

# 安装
add_custom_command(TARGET pyradar_float POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${PYTHON_PACKAGE_INATSLL_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PYTHON_PACKAGE_OUTPUT_DIR}"
    "${PYTHON_PACKAGE_INATSLL_DIR}"
    COMMENT "Copying pyRadar output directory to ${CMAKE_SOURCE_DIR}/.venv/Lib/site-packages/pylfmcwradar"
)

cmake_minimum_required(VERSION 3.15)

# 项目名称和语言
project(pyradar_fixed)

pybind11_add_module(pyradar_fixed
    bind.cc
    source/pyradar_fixed_radar_types.cc
    source/pyradar_fixed_radar_matrix.cc
    source/pyradar_fixed_radar_processer.cc
    source/pyradar_fixed_radar_cfar.cc
)

target_include_directories(pyradar_fixed PRIVATE
    ${PUBLIC_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(pyradar_fixed PRIVATE
    lfmcwradar_fixed
)

set_target_properties(pyradar_fixed
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${PYTHON_PACKAGE_OUTPUT_DIR}"
)

# 生成头文件
add_custom_command(TARGET pyradar_fixed POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=${PYTHON_PACKAGE_OUTPUT_DIR}" ${Python3_EXECUTABLE} -m pybind11_stubgen pyradar_fixed --output-dir ${PYTHON_PACKAGE_OUTPUT_DIR}
    DEPENDS pyradar_fixed
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating .pyi file for pyRadar"
    VERBATIM
)

# 安装
add_custom_command(TARGET pyradar_fixed POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${PYTHON_PACKAGE_INATSLL_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PYTHON_PACKAGE_OUTPUT_DIR}"
    "${PYTHON_PACKAGE_INATSLL_DIR}"
    COMMENT "Copying pyRadar from ${PYTHON_PACKAGE_OUTPUT_DIR} to ${PYTHON_PACKAGE_INATSLL_DIR}"
)

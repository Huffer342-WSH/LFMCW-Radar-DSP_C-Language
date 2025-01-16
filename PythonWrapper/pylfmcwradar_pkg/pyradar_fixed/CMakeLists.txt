cmake_minimum_required(VERSION 3.15)

pybind11_add_module(pyradar_fixed
    bind.cc
    source/pyradar_fixed_radar_cfar.cc
    source/pyradar_fixed_radar_cluster.cc
    source/pyradar_fixed_radar_matrix.cc
    source/pyradar_fixed_radar_processer.cc
    source/pyradar_fixed_radar_types.cc
    source/pyradar_fixed_track_target.cc
    source/pyradar_fixed_track.cc
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
    COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=${PYTHON_PACKAGE_OUTPUT_DIR}" ${Python_EXECUTABLE} -m pybind11_stubgen pyradar_fixed --output-dir ${PYTHON_PACKAGE_OUTPUT_DIR}
    DEPENDS pyradar_fixed
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating .pyi file for pyRadar"
    VERBATIM
)

add_custom_command(TARGET pyradar_fixed POST_BUILD

    # 复制生成的库文件
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:pyradar_fixed>"
    "${PYTHON_PACKAGE_INATSLL_DIR}/$<TARGET_FILE_NAME:pyradar_fixed>"

    # 复制生成的 .pyi 文件
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PYTHON_PACKAGE_OUTPUT_DIR}/pyradar_fixed.pyi"
    "${PYTHON_PACKAGE_INATSLL_DIR}/pyradar_fixed.pyi"

    COMMAND ${CMAKE_COMMAND} -E echo "Copying $<TARGET_FILE_NAME:pyradar_fixed> and pyradar_fixed.pyi from ${PYTHON_PACKAGE_OUTPUT_DIR} to ${PYTHON_PACKAGE_INATSLL_DIR}"
)

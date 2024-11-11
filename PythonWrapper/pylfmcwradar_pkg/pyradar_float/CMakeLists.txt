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
    COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=${PYTHON_PACKAGE_OUTPUT_DIR}" ${Python_EXECUTABLE} -m pybind11_stubgen pyradar_float --output-dir ${PYTHON_PACKAGE_OUTPUT_DIR}
    DEPENDS pyradar_float
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating .pyi file for pyRadar"
    VERBATIM
)


add_custom_command(TARGET pyradar_float POST_BUILD

    # 复制生成的库文件
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:pyradar_float>"
    "${PYTHON_PACKAGE_INATSLL_DIR}/$<TARGET_FILE_NAME:pyradar_float>"

    # 复制生成的 .pyi 文件
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PYTHON_PACKAGE_OUTPUT_DIR}/pyradar_float.pyi"
    "${PYTHON_PACKAGE_INATSLL_DIR}/pyradar_float.pyi"

    COMMAND ${CMAKE_COMMAND} -E echo "Copying $<TARGET_FILE_NAME:pyradar_float> and pyradar_float.pyi from ${PYTHON_PACKAGE_OUTPUT_DIR} to ${PYTHON_PACKAGE_INATSLL_DIR}"
)

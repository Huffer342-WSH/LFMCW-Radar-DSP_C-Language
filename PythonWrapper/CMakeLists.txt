# Windwos平台的Python模块需要使用msvc构建，主要原因是Windows平台和Unix平台的动态链接库调用方式不同，mingw并不兼容

cmake_minimum_required(VERSION 3.5)

# 项目名称和语言
project(pyRadar)

if(MSVC)
    # 关闭警告 C4819
    message(STATUS "Disable warning C4819")
    add_compile_options(
        "/nologo"
        "/wd4819"
    )
endif()

if(WIN32)
    set(PYTHON_EXECUTABLE ${CMAKE_SOURCE_DIR}/.venv/Scripts/python.exe)
else()
    set(PYTHON_EXECUTABLE ${CMAKE_SOURCE_DIR}/.venv/bin/python)
endif()

# 添加 pybind11 子目录（假设它位于项目根目录的 pybind11-2.13.6/ 目录下）
add_subdirectory(pybind11-2.13.6)

pybind11_add_module(pyRadar
    bind.cc
)
target_include_directories(pyRadar PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(pyRadar PRIVATE
    lfmcwradar_float
)

set_target_properties(pyRadar
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${TATGET_OUTPUT_PATH}/python/pyRadar"
)

get_target_property(PYRADAR_OUTPUT_DIR pyRadar LIBRARY_OUTPUT_DIRECTORY)

add_custom_command(TARGET pyRadar POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=${PYRADAR_OUTPUT_DIR}" ${PYTHON_EXECUTABLE} -m pybind11_stubgen pyRadar --output-dir ${PYRADAR_OUTPUT_DIR}
    DEPENDS pyRadar
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating .pyi file for pyRadar"
    VERBATIM
)

add_custom_command(TARGET pyRadar POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PYRADAR_OUTPUT_DIR}"
    "${CMAKE_SOURCE_DIR}/.venv/Lib/site-packages/pylfmcwradar"
    COMMENT "Copying pyRadar output directory to ${CMAKE_SOURCE_DIR}/.venv/Lib/site-packages/pylfmcwradar"
)

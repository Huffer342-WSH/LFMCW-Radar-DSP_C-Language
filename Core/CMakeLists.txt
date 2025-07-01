cmake_minimum_required(VERSION 3.12)

# 检查配置文件所在路径： RADAR_CONFIG_FILE_DIRECTORY
set(RADAR_CONFIG_FILE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Portable" CACHE STRING "Path to RadarConfig.h")
if(RADAR_CONFIG_FILE_DIRECTORY STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}/Portable")
    message(WARNING "Default RADAR_CONFIG_FILE_DIRECTORY is used\n" "If you need to modify the radar algorithm configuration, use the RADAR_CONFIG_FILE_DIRECTORY variable to specify the path where the RadarConfig.h file is located")
    if(CROSS_COMPILE)
        message(FATAL_ERROR "Most of the time, cross-compilation doesn't use the default configuration. Please specify RADAR_CONFIG_FILE_DIRECTORY manually.")
    endif()
else()
    if(EXISTS "${RADAR_CONFIG_FILE_DIRECTORY}/RadarConfig.h")
        message(STATUS "Using RADAR_CONFIG_FILE_DIRECTORY: ${RADAR_CONFIG_FILE_DIRECTORY}")
    else()
        message(FATAL_ERROR "The specified RADAR_CONFIG_FILE_DIRECTORY (${RADAR_CONFIG_FILE_DIRECTORY}) does not contain RadarConfig.h")
    endif()
endif()

string(TIMESTAMP CMAKE_CONFIGURE_TIME "%Y-%m-%d %H:%M:%S")
configure_file(
    ${PROJECT_SOURCE_DIR}/CMake/template/version.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/radar/version.h
)


include_directories(
    ${RADAR_CONFIG_FILE_DIRECTORY}
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

add_subdirectory(Source)


# 添加目标
add_library(RadarDP_Fixed STATIC)

target_link_libraries(RadarDP_Fixed PUBLIC
    RadarDP_Common_objs
    RadarDP_SigProc_fixed_objs
    RadarDP_ObjTrack_objs
    RadarDP_Processer_fixed_objs
)

target_include_directories(RadarDP_Fixed PUBLIC
    ${RADAR_CONFIG_FILE_DIRECTORY}
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

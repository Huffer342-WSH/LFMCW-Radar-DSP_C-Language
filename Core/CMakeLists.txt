cmake_minimum_required(VERSION 3.12)

# # 检查板级路径
if(NOT TARGET RadarBoard)
    message(FATAL_ERROR "RadarBoard target not found. Please ensure the RADAR_BOARD_PATH is correctly set and the corresponding CMakeLists.txt is included.")
    exit()
endif()

link_libraries(RadarBoard)

# 生成头文件version.h
string(TIMESTAMP CMAKE_CONFIGURE_TIME "%Y-%m-%d %H:%M:%S")
configure_file(
    ${PROJECT_SOURCE_DIR}/CMake/template/version.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/radar/version.h
)

# 添加全局头文件
include_directories(
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
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

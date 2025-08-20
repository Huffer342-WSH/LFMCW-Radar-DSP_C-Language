cmake_minimum_required(VERSION 3.12)

add_library(RadarDP_Processer_fixed_objs OBJECT)
add_library(RadarDP_Processer_fixed STATIC)

target_link_libraries(RadarDP_Processer_fixed_objs PUBLIC
    CMSISDSP
)

target_sources(RadarDP_Processer_fixed_objs PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/processer_fixed.c
)

target_link_libraries(RadarDP_Processer_fixed PUBLIC
    RadarDP_Processer_fixed_objs
)

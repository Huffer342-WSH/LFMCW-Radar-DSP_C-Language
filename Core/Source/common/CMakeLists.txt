cmake_minimum_required(VERSION 3.12)

add_library(RadarDP_Common_objs OBJECT)
add_library(RadarDP_Common STATIC)

target_sources(RadarDP_Common_objs PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/radar_error.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mm.c
)

target_link_libraries(RadarDP_Common PUBLIC
    RadarDP_Common_objs
)

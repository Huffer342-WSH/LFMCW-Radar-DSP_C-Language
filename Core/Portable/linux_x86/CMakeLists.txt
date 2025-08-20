add_library(RadarBoard STATIC)

target_include_directories(RadarBoard PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_sources(RadarBoard PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/board.c
)

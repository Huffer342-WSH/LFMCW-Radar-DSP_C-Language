add_library(RadarSP_float STATIC)

target_link_libraries(RadarSP_float PUBLIC
    CMSISDSP
    common
)

target_sources(RadarSP_float PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/radar_cfar.c
)

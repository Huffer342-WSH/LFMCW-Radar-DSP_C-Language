cmake_minimum_required(VERSION 3.12)

add_library(RadarDP_ObjTrack_objs OBJECT)
add_library(RadarDP_ObjTrack STATIC)

target_include_directories(RadarDP_ObjTrack_objs PRIVATE
    ${PROJECT_SOURCE_DIR}/Source/Include/radar/ot
    ${PROJECT_SOURCE_DIR}/Source/Include/radar/sp
    ${PROJECT_SOURCE_DIR}/Source/Include/radar/common
)

target_link_libraries(RadarDP_ObjTrack_objs PUBLIC
    Eigen3::Eigen
    scipy
    RadarDP_Common
)

target_sources(RadarDP_ObjTrack_objs PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/track_associator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/track_deleter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/track_initiator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/track_kalman.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/track_model.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/track_target.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/track_tracking.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/track.cc
)


if(${CMAKE_CXX_COMPILER_ID} STREQUAL "ARMClang")
    target_compile_options(RadarDP_ObjTrack_objs PRIVATE -ffp-mode=full)
endif()



target_link_libraries(RadarDP_ObjTrack PUBLIC
    RadarDP_ObjTrack_objs
)

set(HOST off)
set(CMSISCORE ${CMAKE_SOURCE_DIR}/ThirdParty/CMSIS-Core)
add_subdirectory(${CMAKE_SOURCE_DIR}/ThirdParty/CMSIS-DSP/Source)
add_subdirectory(${CMAKE_SOURCE_DIR}/ThirdParty/STC-4.2)
add_subdirectory(${CMAKE_SOURCE_DIR}/Source)

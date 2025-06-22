# 重要依赖，必须

if(CROSS_COMPILE)
    set(CMSISCORE ${PROJECT_SOURCE_DIR}/ThirdParty/CMSIS-Core)
    set(HOST OFF)
    add_subdirectory(CMSIS-DSP/Source EXCLUDE_FROM_ALL)
else()
    set(HOST ON)
    add_subdirectory(CMSIS-DSP/Source EXCLUDE_FROM_ALL)
endif()

add_subdirectory(eigen EXCLUDE_FROM_ALL)
add_subdirectory(scipy EXCLUDE_FROM_ALL)

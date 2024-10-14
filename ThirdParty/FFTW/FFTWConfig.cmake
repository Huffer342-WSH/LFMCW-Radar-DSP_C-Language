# FFTWConfig.cmake

# Define FFTW include directory
set(PLATFORM_SUBDIRECTORY "${CMAKE_CURRENT_LIST_DIR}/windows-FFTW-3.3.5")
set(FFTW_INCLUDE_DIR "${PLATFORM_SUBDIRECTORY}/include")

# Define FFTW libraries
set(FFTW_LIBRARIES "${PLATFORM_SUBDIRECTORY}/lib/libfftw3-3.dll"
    "${PLATFORM_SUBDIRECTORY}/lib/libfftw3f-3.dll"
    "${PLATFORM_SUBDIRECTORY}/lib/libfftw3l-3.dll")

# Check if the FFTW header file exists
if(NOT EXISTS "${FFTW_INCLUDE_DIR}/fftw3.h")
    message(FATAL_ERROR "Could not find fftw3.h in ${FFTW_INCLUDE_DIR}")
endif()

# Create an interface target for FFTW
add_library(FFTW INTERFACE)
target_include_directories(FFTW INTERFACE ${FFTW_INCLUDE_DIR})
target_link_libraries(FFTW INTERFACE ${FFTW_LIBRARIES})

# Provide FFTW to consumers
set(FFTW_FOUND TRUE)

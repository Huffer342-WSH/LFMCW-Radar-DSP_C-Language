if(NOT DEFINED Python_EXECUTABLE)
    message(FATAL_ERROR "Python_EXECUTABLE is not set, use find_package(Python) before")
endif()



add_subdirectory(fixed_point)

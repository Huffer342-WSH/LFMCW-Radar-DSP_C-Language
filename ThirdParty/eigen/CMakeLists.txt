if(CONFIGURE_EIGEN)
    add_subdirectory(eigen-3.4.0)
else()
    add_library(Eigen3 INTERFACE)

    target_include_directories(Eigen3 INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/eigen-3.4.0"
    )

    add_library(Eigen3::Eigen ALIAS Eigen3)
endif()

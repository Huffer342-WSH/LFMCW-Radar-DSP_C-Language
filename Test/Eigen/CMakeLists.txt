project(Test_Eigen C CXX)

add_executable(test_matrix test_matrix.c
    ceigen.cc
)

target_link_libraries(test_matrix Eigen3::Eigen)

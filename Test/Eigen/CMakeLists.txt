project(Test_Eigen C CXX)

add_executable(test_matrix test_matrix.c
    ceigen.cc
)

target_link_libraries(test_matrix Eigen3::Eigen)

# EIGEN赋值测试
add_executable(test_eigen_assignment
    test_eigen_assignment.cc
)

target_link_libraries(test_eigen_assignment Eigen3::Eigen)

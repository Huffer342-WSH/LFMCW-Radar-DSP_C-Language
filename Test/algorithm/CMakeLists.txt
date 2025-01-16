
add_executable(test_ekf mat_inv.c)

add_executable(test_dgemm dgemm.c)

add_executable(test_lsap
    test_rectangular_lsap.cc
)

target_link_libraries(test_lsap scipy)

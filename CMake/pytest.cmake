add_test(
    NAME test_python_module
    COMMAND ${Python3_EXECUTABLE} -m pytest ${CMAKE_SOURCE_DIR}/Test/Python/fixed_point
)

# 设置失败时的返回码，0 表示成功，非 0 表示失败
set_tests_properties(test_python_module PROPERTIES
    FAIL_REGULAR_EXPRESSION "FAILED"
)

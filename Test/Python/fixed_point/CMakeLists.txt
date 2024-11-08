add_test(
    NAME PythonModuleTest.fixed.cfar
    COMMAND ${Python3_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/test_cfar.py
)

add_test(
    NAME PythonModuleTest.fixed.matrix
    COMMAND ${Python3_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/test_matrix.py
)

add_test(
    NAME PythonModuleTest.fixed.cfar
    COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/test_cfar.py
)

add_test(
    NAME PythonModuleTest.fixed.matrix
    COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/test_matrix.py
)


add_test(
    NAME PythonModuleTest.fixed.dbscan
    COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/test_cluster.py
)

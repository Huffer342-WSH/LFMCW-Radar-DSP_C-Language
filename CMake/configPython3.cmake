# 指定虚拟环境的 Python 路径（假设虚拟环境路径为 venv）
set(Python3_ROOT_DIR "${CMAKE_SOURCE_DIR}/.venv")

# 查找 Python3，确保找到的是虚拟环境中的 Python 解释器
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# 检查是否找到 Python3 解释器
if(Python3_FOUND)
    message(STATUS "Found Python interpreter: ${Python3_EXECUTABLE}")
    message(STATUS "Python site-packages path: ${Python3_SITEARCH}")

else()
    message(FATAL_ERROR "Python interpreter not found in venv.")
endif()

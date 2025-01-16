message(STATUS "[Config Python]::Begin")

# 指定虚拟环境的 Python 路径（假设虚拟环境路径为 venv）
set(Python_ROOT_DIR "${PROJECT_SOURCE_DIR}/.venv")

# 查找 Python3，确保找到的是虚拟环境中的 Python 解释器
find_package(Python REQUIRED COMPONENTS Interpreter Development)

# 检查是否找到 Python3 解释器
if(Python_FOUND)
    # 为pybind11设置PYTHON_EXECUTABLE和PYTHON_LIBRARIES
    set(PYTHON_EXECUTABLE ${Python_EXECUTABLE})
    set(PYTHON_LIBRARIES ${Python_LIBRARIES})
    message(STATUS "Python version: ${Python_VERSION}")
    message(STATUS "Found Python interpreter: ${Python_EXECUTABLE}")
    message(STATUS "Python site-packages path: ${Python_SITEARCH}")
    message(STATUS "Python libraries: ${Python_LIBRARIES}")
    message(STATUS "Python3 found")
else()
    message(FATAL_ERROR "Python interpreter not found in .venv.")
endif()

message(STATUS "[Config Python]::End\n")

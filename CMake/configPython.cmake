

function(get_config_value file_content key_name out_var)
    # 构造正则表达式，匹配 key_name= 后的内容
    string(REGEX MATCH "${key_name}=(.*)" match ${file_content})
    # 如果匹配成功，则设置返回变量
    if(match)
        set(${out_var} ${CMAKE_MATCH_1} PARENT_SCOPE)
    else()
        set(${out_var} "" PARENT_SCOPE)
    endif()
endfunction()

message(STATUS "[Config Python]::Begin")
# 指定虚拟环境的 Python 路径（假设虚拟环境路径为 venv）
file(READ "${PROJECT_SOURCE_DIR}/.venv/.info" VENV_INFO)
message("VENV_INFO:\n${VENV_INFO}\n")
get_config_value(${VENV_INFO} "venv_path" Python_ROOT_DIR)
message("Python_ROOT_DIR: ${Python_ROOT_DIR}")


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
    message(FATAL_ERROR "Python interpreter not found in .venv. Try
     to tun setup_venv.py to creat a venv")
    
endif()

message(STATUS "[Config Python]::End\n")

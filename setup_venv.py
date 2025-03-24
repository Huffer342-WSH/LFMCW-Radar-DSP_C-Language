import os
import subprocess
import sys
import shutil
from pathlib import Path
import platform

# 指定需要安装的库列表
REQUIRED_PACKAGES = [
    "pytest",
    "numpy",
    "scipy",
    "ipython",
    "ipykernel",
    "pybind11",
    "pybind11-stubgen",
    "nbformat",
    "joblib",
    "moviepy",
    "plotly",
    "scikit-learn",
]


def get_python_version(venv_python):
    try:
        # 获取虚拟环境中的 Python 版本
        version = subprocess.run([venv_python, "--version"], capture_output=True, text=True).stdout.strip()
        return version.split()[-1]  # 返回版本号
    except Exception as e:
        print(f"获取虚拟环境的Python版本失败: {e}")
        return None


def create_virtualenv(venv_path):
    print(f"创建新的虚拟环境: {venv_path}")
    subprocess.run([sys.executable, "-m", "venv", venv_path])


def install_packages(venv_python, packages):
    # 安装指定的库
    print(f"在虚拟环境中安装库: {', '.join(packages)}")
    subprocess.run([venv_python, "-m", "pip", "install", "--upgrade", "pip"])  # 先升级 pip
    subprocess.run([venv_python, "-m", "pip", "install"] + packages)


def main():
    python_relative_path_map = {
        "Windows": "Scripts/python.exe",
        "Linux": "bin/python",
    }
    
    system_name = platform.system()
    print(f"当前系统：{system_name}")

    
   

    # .venv 路径
    venv_path = Path(f".venv/{system_name}")
    print(f".venv路径:{venv_path}")
    

    current_python_version = sys.version.split()[0]
    print(f"当前系统的Python版本: {current_python_version}")
    
    python_relative_path = python_relative_path_map[system_name]
    venv_python = Path(f"{venv_path}/{python_relative_path}")

    if venv_path.exists():
        print(".venv 文件夹已存在")
        print(f"搜索Python interpreter: {venv_python}")
        
        venv_version = get_python_version(venv_python)
        
        if venv_version == current_python_version:
            print(f"虚拟环境的Python版本为 {venv_version}，与当前系统匹配")
        else:
            print(f"虚拟环境的Python版本为 {venv_version}，与当前系统不匹配")
            print("删除旧的虚拟环境")
            shutil.rmtree(venv_path)
            create_virtualenv(venv_path)
    else:
        print(".venv 文件夹不存在，创建新的虚拟环境")
        create_virtualenv(venv_path)

    # 安装所需的库
    install_packages(venv_python, REQUIRED_PACKAGES)
    
    venv_info_path = Path(f".venv/.info")
    venv_info_content = f"{venv_path}"
    venv_info_content = dict()
    venv_info_content["venv_path"] =  os.path.abspath(venv_path)
    with open(venv_info_path, "w", encoding="utf-8") as file:
        for key,valye in venv_info_content.items():
            file.write(f"{key}={valye}")


if __name__ == "__main__":
    main()

import os
import subprocess
import sys
import shutil
from pathlib import Path

# 指定需要安装的库列表
REQUIRED_PACKAGES = ["numpy", "scipy", "ipython", "ipykernel", "pybind11", "pybind11-stubgen", "nbformat", "joblib", "moviepy", "plotly"]


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
    current_python_version = sys.version.split()[0]
    print(f"当前系统的Python版本: {current_python_version}")

    # .venv 路径
    venv_path = Path(".venv")
    venv_python = venv_path / "bin" / "python" if os.name != "nt" else venv_path / "Scripts" / "python.exe"

    if venv_path.exists():
        print(".venv 文件夹已存在")
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


if __name__ == "__main__":
    main()

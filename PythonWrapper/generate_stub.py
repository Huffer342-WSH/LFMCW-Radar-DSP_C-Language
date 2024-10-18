import os
import subprocess
import sys

def generate_pyi(pyd_path, module_name, output_dir):
    # 获取当前的环境变量并更新 PYTHONPATH
    env = os.environ.copy()
    env['PYTHONPATH'] = pyd_path + os.pathsep + env.get('PYTHONPATH', '')
    
    # 创建生成的 pyi 文件的输出目录
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    # 生成 stub 文件的命令
    command = [
        sys.executable, '-m', 'pybind11_stubgen',
        '--output-dir', output_dir,
        module_name
    ]
    
    # 运行子进程，生成 .pyi 文件
    try:
        subprocess.run(command, check=True, env=env)
        print(f"Successfully generated .pyi file for module '{module_name}' in '{output_dir}'")
    except subprocess.CalledProcessError as e:
        print(f"Error while generating .pyi file: {e}")

if __name__ == '__main__':
    # 示例使用：
    # pyd 文件所在目录
    pyd_path = os.path.realpath("../.output/Debug/python/pyRadar")
    
    # 模块名 (例如 your_module 名字与 your_module.pyd 一致)
    module_name = "pyRadar"
    
    # 输出的目录
    output_dir = pyd_path
    
    generate_pyi(pyd_path, module_name, output_dir)

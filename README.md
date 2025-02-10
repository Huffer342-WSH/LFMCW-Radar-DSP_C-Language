# LFMCW雷达信号处理——C/C++语言实现

## 1.简介

该项目旨在使用C/C++语言实现一个可移植的适用于LFMCW雷达的目标检测程序，包含信号处理和目标跟踪等功能。

该项目当前开发平台为Windwos，目标平台Cortex-M0+。项目中的内容大致分为核心算法与测试案例。
- 核心算法仅需要复制粘贴就可以移植到单片机上的核心算法代码，
- 测试案例则包含C++实现的单元测试和验证整体效果用的Python代码
  - 核心算法编译出来的静态库封装成Python模块，在Python中测试算法效果，可以更方便实现数据可视化。


**文件结构**：
```
├──.output                  # 生成的可执行文件和库文件
├──.venv                    # Python虚拟环境
├──.vscode                  # VSCode配置
├──Data                     # 测试数据
├──Doc                      # 文档
├──PythonWrapper            # C语言静态库封装成Python模块
│   └── pylfmcwradar_pkg    # Python模块源代码
│       ├── include         # 公共头文件，提供封装数据结构用的模板
│       ├── pyradar_fixed   # 定点数版本封装源代码
│       └── pyradar_float   # 浮点数版本模块源代码
├──Source                   # C语言源码
│  ├──fixed_point           # 定点数版本，当前继续该版本用于Cortex-M0平台
│  └──float_point           # 浮点数版本
├──Test                     # 测试程序，测试部分函数以及静态库
│   ├── CMSIS-DSP
│   ├── FFTW
│   ├── Python              # Python模块测试
│   └── Radar_Fixed         # 雷达算法库点数版本的C语言测试
└──ThirdParty    # 第三方库
    ├── CMSIS-DSP           # 适合ARM的DSP函数
    ├── eigen-3.4.0         # 矩阵运算（卡尔曼滤波基于Eigen实现）
    ├── googletest-1.15.2   # C++测试框架
    ├── pybind11-2.13.6     # 封装C语言代码成Python模块
    └── ...
```

## 2. 使用该项目

[《构建与使用.md》](.Doc/构建与使用.md)

## 3. Git代码管理

[《Git代码管理》](Doc/Git代码管理.md)

## 4. 开发进度

[《版本简述》](./Doc/版本简述.md)

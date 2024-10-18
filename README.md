# LFMCW雷达信号处理——C语言实现

## 1. 准备工具

- Python： 用来创建venv
- gcc/MinGW： 虽然Python官方推荐在Windows平台下使用MSVC构建模块，但是MinGW貌似也行。
  > MSVC不支持可变长度数组，不是很方便。
- CMake

## 2.构建

编译出的文件位于项目根目录下的`.output`文件夹中
```
.output
 └───<build_type> # 例如：Debug、Release
     ├───bin      # 可执行文件
     ├───lib      # 库
     └───python   # python模块
```

构建pyRadar还会复制python模块到`.venv/Lib/site-packages/pylfmcwradar`目录中


### 2.1 创建python虚拟环境

```shell
python setup_venv.py
```

我使用的是python3.12.7。因为该项目会构建Python模块并安装，所以推荐创建虚拟环境，避免污染全局环境。


### 2.2 CMake Configure

```shell
cmake -S . -B build
```

### 2.3 CMake Build

默认编译选项
```shell
cmake --build ./build -j
```

第三方库的测试默认编译，编译选项如下

```shell
cmake --build ./build --target test-fftw -j
cmake --build ./build --target test-cmsisdsp -j
```

## 3.测试

。。。

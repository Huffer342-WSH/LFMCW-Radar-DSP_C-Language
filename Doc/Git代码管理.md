# 代码管理

git pull最新的develop分支，然后新建并签出分支，分支命名格式为 `feature/<功能>` ，如 `feature/cfar2d-fixed-point-edition`


## 开发新功能

开发一个功能函数的流程如下
1. 编写C语言源码
2. 使用C/C++编写单元测试，测试C语言函数
3. 将新编写的C语言函数封装成Python模块中的函数，一些复杂的功能可以跳过这一步，使用Python测试
4. 在`Test/Python/<edition>/test_<file>.py`中添加测试，测试模块封装以及函数功能
5. 将新功能添加到`Test/Python/<edition>/test_main.py`中，测试完整的雷达算法流程

## 合并分支

1. 拉取最新的develop分支 `git pull --rebase` 
2. 将功能分支变基到最新的develop分支上，可能需要解决冲突 
   ```shell
   git rebase develop
   ```
3. 将功能分支合并到develop分支，因为上一步执行了变基操作，所以这次合并不会新建一个`合并提交`。
   ```shell
   git checkout develop
   git merge <feature>
   ```

import pytest
import numpy as np
from pylfmcwradar import pyradar_fixed as pyRadar


# 测试：验证 matrix3d_complex_int16_alloc 是否正确分配了矩阵
def test_matrix3d_complex_int16_alloc():
    # 创建一个10x10x10的矩阵
    mat = pyRadar.matrix3d_complex_int16_alloc(10, 10, 10)

    # 检查对象是否创建成功
    assert mat is not None, "Matrix allocation failed!"

    # 检查矩阵的大小
    assert mat.size0 == 10, f"Expected size0 to be 10, but got {mat.size0}"
    assert mat.size1 == 10, f"Expected size1 to be 10, but got {mat.size1}"
    assert mat.size2 == 10, f"Expected size2 to be 10, but got {mat.size2}"

    # 检查所有成员变量
    assert mat.tda1 == 100
    assert mat.tda2 == 10
    assert mat.owner == 1


# 测试：验证 `data` 属性的读取和写入
def test_matrix3d_complex_int16_data_property():
    # 创建一个矩阵实例
    mat = pyRadar.matrix3d_complex_int16_alloc(10, 10, 10)

    # 获取矩阵数据
    data = mat.data

    # 确保返回的数据是一个NumPy数组
    assert isinstance(data, np.ndarray), "Expected data to be a NumPy array"

    # 检查数据的形状
    assert data.shape == (10, 10, 10, 2), f"Expected data shape (10, 10, 10, 2), but got {data.shape}"

    # 检查单个索引赋值
    ref = np.random.randint(-32768, 32768, (10, 10, 10, 2), dtype=np.int16)
    for i in range(10):
        for j in range(10):
            for k in range(10):
                mat.data[i, j, k, 0] = ref[i, j, k, 0]
                mat.data[i, j, k, 1] = ref[i, j, k, 1]
    assert np.array_equal(mat.data, ref), "Data was not correctly modified!"

    # 检查切片赋值
    ref = np.random.randint(-32768, 32768, (10, 10, 10, 2), dtype=np.int16)
    mat.data[:] = ref
    assert np.array_equal(mat.data, ref), "Data was not correctly modified!"

    # 验证数据是否已被修改
    assert np.array_equal(mat.data, ref), "Data was not correctly modified!"

    # 修改数据并设置回矩阵
    new_data = np.zeros((10, 10, 10, 2), dtype=np.int16)
    mat.data = new_data

    # 验证数据是否已被更新
    assert np.array_equal(mat.data, new_data), "Data was not correctly set!"

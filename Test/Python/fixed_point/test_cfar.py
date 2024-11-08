# %%
import pytest
import numpy as np
from pylfmcwradar import pyradar_fixed as pyRadar

# %%


def test_cfar_result_alloc_and_readwrite():
    cfar_result = pyRadar.cfar2d_result_alloc(10)

    # 测试numPoint属性读写
    cfar_result.numPoint = 1
    assert cfar_result.numPoint == 1
    assert len(cfar_result.getList()) == 1
    cfar_result.numPoint = 2
    assert cfar_result.numPoint == 2
    assert len(cfar_result.getList()) == 2

    # 测试通过numpy结构体数组访问数据
    cfar_result.point[0]["amp"] = 1
    cfar_result.point[0]["idx0"] = 2
    cfar_result.point[0]["idx1"] = 3
    cfar_result.point[0]["snr"] = 4

    assert cfar_result.point[0]["amp"] == 1
    assert cfar_result.point[0]["idx0"] == 2
    assert cfar_result.point[0]["idx1"] == 3
    assert cfar_result.point[0]["snr"] == 4

    # 测试通过list访问数据
    cfar_result_list = cfar_result.getList()
    assert cfar_result_list[0].amp == 1
    assert cfar_result_list[0].idx0 == 2
    assert cfar_result_list[0].idx1 == 3
    assert cfar_result_list[0].snr == 4

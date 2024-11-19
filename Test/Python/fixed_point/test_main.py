# %%
import sys

sys.path.append("../")

from pylfmcwradar import pyradar_fixed as pyRadar
import pytest
import numpy as np
import scipy
from scipy.fft import fft, fft2, fftshift
import plotly.graph_objects as go
from drawhelp import draw as dh


# %%
def set_matrix3d_complex_int16(m: pyRadar.matrix3d_complex_int16, complexMat: np.ndarray):
    """将复数矩阵转换加载到C语言matrix3d_complex_int16中"""
    m.data[:] = np.stack((complexMat.real, complexMat.imag), axis=-1).astype(np.int16)


# %% 加载数据

mat = scipy.io.loadmat(file_name="../../../Data/RadarData_Simulate.mat")

c = scipy.constants.c
frequency = mat["frequency"][0, 0]
wavelength = c / frequency
bandwidth = mat["bandwidth"][0, 0]
timeChrip = mat["timeChrip"][0, 0]
timeChripGap = mat["timeChripGap"][0, 0]
timeFrameGap = mat["timeFrameGap"][0, 0]
numPoint = mat["numPoint"][0, 0]
numRangeBin = 25
numChrip = mat["numChrip"][0, 0]
numChannel = mat["numChannel"][0, 0]
numFrame = len(mat["radarDataCube"])

rdms_list = fft(fft(mat["radarDataCube"], axis=-1)[:, :, :, :numRangeBin], axis=-2).transpose(0, 1, 3, 2)
rdms_list = rdms_list * (2**15 - 1) / np.max(np.abs(rdms_list))
rdms_list.real = rdms_list.real.astype(np.int16)
rdms_list.imag = rdms_list.imag.astype(np.int16)


# %% 初始化雷达算法
radar_init_param = pyRadar.radar_init_param()
radar_handle = pyRadar.radar_handle()
rdms = pyRadar.matrix3d_complex_int16_alloc(numChannel, numRangeBin, numChrip)
noise_buffer = pyRadar.matrix2d_int32_alloc(numRangeBin, numChrip)


radar_init_param.wavelength = wavelength
radar_init_param.bandwidth = bandwidth
radar_init_param.timeChrip = timeChrip
radar_init_param.timeChripGap = timeChripGap
radar_init_param.timeFrameGap = timeFrameGap
radar_init_param.numChannel = numChannel
radar_init_param.numRangeBin = numRangeBin
radar_init_param.numChrip = numChrip
radar_init_param.numMaxCfarPoints = 20
radar_init_param.numMaxCachedFrame = 8
radar_init_param.numInitialMultiMeas = 40
radar_init_param.numInitialCluster = 5

radar_config = pyRadar.radar_config()
radar_config.cfarCfg.numGuard[0] = 1
radar_config.cfarCfg.numGuard[1] = 1
radar_config.cfarCfg.numTrain[0] = 2
radar_config.cfarCfg.numTrain[1] = 3
radar_config.cfarCfg.thAmp = 0
radar_config.cfarCfg.thSNR = 2.0

radar_config.cfar_filter_cfg.range0 = 1
radar_config.cfar_filter_cfg.range1 = 3
radar_config.cfar_filter_cfg.shape1 = numChrip
radar_config.cfar_filter_cfg.thSNR = 0.6

radar_config.dbscan_config.wr = int(1.0 * (1 << 16))
radar_config.dbscan_config.wv = 0
radar_config.dbscan_config.eps = 700
radar_config.dbscan_config.min_samples = 2

print(f"雷达初始化参数:\n{radar_init_param}")
pyRadar.radardsp_init(radar_handle, radar_init_param, radar_config)


# %% 输入一个帧
def add_one_frame(frame):
    set_matrix3d_complex_int16(rdms, rdms_list[0])
    pyRadar.radardsp_input_new_frame(radar_handle, rdms)
    pyRadar.radar_cfar2d_goca_debug(noise_buffer, radar_handle.basic.magSpec2D, radar_handle.config.cfarCfg)
    mag = radar_handle.getMagSpec2D()
    count = radar_handle.cfar.numPoint
    indices = np.column_stack((radar_handle.cfar.point[:count]["idx0"], radar_handle.cfar.point[:count]["idx1"]))
    return (mag, indices)


# %% 输入所有帧

magSpec2D_list = np.zeros((len(rdms_list), numRangeBin, numChrip))
magSpec2DRef_list = np.zeros((len(rdms_list), numRangeBin, numChrip))
noise_list = np.zeros((len(rdms_list), numRangeBin, numChrip))
indicesList = []  # 每个元素代表一帧，帧为一个Nx2的矩阵，记录N个RDM中的坐标
measList = []
clusterList = []
for i, frame in enumerate(rdms_list):
    print(f"第{i}帧", flush=True)
    set_matrix3d_complex_int16(rdms, frame)
    pyRadar.radardsp_input_new_frame(radar_handle, rdms)

    pyRadar.radar_cfar2d_goca_debug(noise_buffer, radar_handle.basic.magSpec2D, radar_handle.config.cfarCfg)

    magSpec2D_list[i] = radar_handle.getMagSpec2D()
    magSpec2DRef_list[i] = np.sum(np.abs(frame) * np.sqrt(2**25), axis=0)
    noise_list[i] = noise_buffer.data.copy()
    count = radar_handle.cfar.numPoint
    indicesList.append(np.column_stack((radar_handle.cfar.point[:count]["idx0"], radar_handle.cfar.point[:count]["idx1"])))
    meas = radar_handle.getMeasurements()
    measList.append(
        np.column_stack(
            (
                meas[:]["distance"],
                meas[:]["velocity"],
                meas[:]["azimuth"],
            )
        )
    )
    meas = radar_handle.getClusterMeasurements()
    clusterList.append(
        np.column_stack(
            (
                meas[:]["distance"],
                meas[:]["velocity"],
                meas[:]["azimuth"],
            )
        )
    )
    # print(indicesList[-1])
    # print(measList[-1])

snr_list = magSpec2D_list / noise_list

mese = np.mean([np.mean(np.abs(magSpec2D_list[i] - magSpec2DRef_list[i]) ** 2) for i in range(len(magSpec2D_list))])
print("幅度谱均方误差：", mese)

# %%
dh.draw_2d_spectrumlist(magSpec2D_list[::100], title="幅度谱").show()
dh.draw_2d_spectrumlist(magSpec2DRef_list[::100], title="幅度谱").show()
dh.draw_2d_spectrumlist(snr_list[::100], title="幅度谱").show()

# %%
""" 绘制RDM的CFAR搜索结果 """
listData = []
resRange = float(radar_handle.param.resRange)
resVelocity = float(radar_handle.param.resVelocity)
for i in range(numFrame):
    data = list()
    data.append(go.Scatter(x=indicesList[i][:, 0], y=indicesList[i][:, 1], mode="markers", name="cfar"))
    data.append(go.Scatter(x=measList[i][:, 0] / resRange, y=measList[i][:, 1] / resVelocity, mode="markers", name="超分辨率"))
    listData.append(data)
fig = dh.draw_animation(listData[::1], title="RDM的 GOCA-2DCFAR 搜索结果")
fig.update_layout(
    xaxis=dict(range=[-1, numRangeBin + 1]),
    yaxis=dict(range=[-1, numChrip + 1]),
    title="点云",
)
fig.show()

# %%
"""计算二维平面点云"""
pointCloudList = []
targetList = []
for i in measList:
    pointClouds = np.column_stack((i[:, 0] * np.cos(i[:, 2] / 2**13), i[:, 0] * np.sin(i[:, 2] / 2**13)))
    pointClouds /= 1000
    pointCloudList.append(pointClouds)
for i in clusterList:
    cluster = np.column_stack((i[:, 0] * np.cos(i[:, 2] / 2**13), i[:, 0] * np.sin(i[:, 2] / 2**13)))
    cluster /= 1000
    targetList.append(cluster)

# %%
""" 绘制二维平面点云 """
listData = []
resRange = float(radar_handle.param.resRange)
resVelocity = float(radar_handle.param.resVelocity)
for i in range(numFrame):
    data = list()
    data.append(go.Scatter(x=pointCloudList[i][:, 0], y=pointCloudList[i][:, 1], mode="markers", name="cfar"))
    data.append(go.Scatter(x=targetList[i][:, 0], y=targetList[i][:, 1], mode="markers", name="dbscan"))
    listData.append(data)
fig = dh.draw_animation(listData[::5], title="RDM的 GOCA-2DCFAR 搜索结果")
fig.update_layout(
    xaxis=dict(range=[0, 15]),
    yaxis=dict(range=[-10, 10]),
    title="点云",
)
fig.show()

# %% 绘制一帧数据
# i = 176
# go.Figure(data=[go.Surface(z=magSpec2D_list[i])]).show()
# go.Figure(data=[go.Surface(z=magSpec2DRef_list[i])]).show()
# go.Figure(data=[go.Surface(z=noise_list[i])]).show()
# go.Figure(data=[go.Surface(z=snr_list[i])]).show()


# %%

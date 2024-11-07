# %%

from pylfmcwradar import pyradar_float as pyRadar
import numpy as np
import scipy.io
from scipy.fft import fft, fft2, fftshift
import plotly.graph_objects as go
import drawhelp.draw as dh

# 注意使用np.ascontiguousarray保证numpy数组在内存中的连续性，否则C语言模块无法兼容


radarHandle = pyRadar.RadarHandle()
pyRadar.radardsp_init(radarHandle)
# %%
print(
    f"\ntype of radarParam.numPoint :{type(radarHandle.basic.staticClutter)}\n"
    f"radarHandle.param.numChannel: {radarHandle.param.numChannel}\n"
    f"radarHandle.param.numRangeBin: {radarHandle.param.numRangeBin}\n"
    f"radarHandle.param.numChrip: {radarHandle.param.numChrip}\n"
)


# %%
# 导入数据

ENABLE_CPROFILE = False


mat = scipy.io.loadmat(
    file_name="../../data/RadarData_Simulate.mat",
)

# 提取信号，添加直流偏置和噪声
radarDataCube = mat["radarDataCube"]
# radarDataCube += 1e-4 * (np.random.randn(*radarDataCube.shape) + 1j * np.random.randn(*radarDataCube.shape)) # 白噪声
radarDataCube += np.random.randn(radarDataCube.shape[-1]) + 1j * np.random.randn(radarDataCube.shape[-1])  # 直流偏置

frequency = mat["frequency"][0, 0]
bandwidth = mat["bandwidth"][0, 0]
timeChrip = mat["timeChrip"][0, 0]
timeChripGap = mat["timeChripGap"][0, 0]
timeFrameGap = mat["timeFrameGap"][0, 0]
numPoint = mat["numPoint"][0, 0]
numRangeBin = 25
numChrip = mat["numChrip"][0, 0]
numChannel = mat["numChannel"][0, 0]
print(
    f"\nfrequency: {frequency}"
    f"\nbandwidth: {bandwidth}"
    f"\ntimeChrip: {timeChrip}"
    f"\ntimeChripGap: {timeChripGap}"
    f"\ntimeFrameGap: {timeFrameGap}"
    f"numPoint: {numPoint}"
    f"\nnumRangeBin: {numRangeBin}"
    f"\nnumChrip: {numChrip}"
    f"\nnumChannel: {numChannel}"
    "\n"
)

referPositionList = mat["tergatTrajectory"][:, :, :2].transpose(1, 0, 2)

chripMean = fft2(radarDataCube[0], axes=(-2, -1))[:, 0, :]

# %%
# 使用C语言模块处理数据k,同时把数据保存下来观察

magSpec2DList = []
for frame in radarDataCube:
    rdms = fft(fft(frame, axis=-1)[:, :, :numRangeBin], axis=-2).transpose(0, 2, 1)
    pyRadar.radardsp_input_new_frame(radarHandle, rdms)
    magSpec2DList.append(radarHandle.basic.magSpec2D)
    magSpec2D = np.abs(rdms[0])
    radarHandle.basic.magSpec2D[0][0]
    # print(f"误差： {np.mean(np.abs(radarHandle.basic.magSpec2D - magSpec2D))}")
dh.draw_2d_spectrumlist(magSpec2DList[::40], title="幅度谱")
# %%

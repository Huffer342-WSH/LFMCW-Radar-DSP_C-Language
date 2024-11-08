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

rdms_list = fft(fft(mat["radarDataCube"], axis=-1)[:, :, :, :numRangeBin], axis=-2).transpose(0, 1, 3, 2)
rdms_list = rdms_list * (2**15 - 1) / np.max(np.abs(rdms_list))
rdms_list.real = rdms_list.real.astype(np.int16)
rdms_list.imag = rdms_list.imag.astype(np.int16)


# %% 初始化雷达算法
radar_init_param = pyRadar.radar_init_param()
radar_handle = pyRadar.radar_handle()
rdms = pyRadar.matrix3d_complex_int16_alloc(numChannel, numRangeBin, numChrip)

radar_init_param.wavelength = wavelength
radar_init_param.bandwidth = bandwidth
radar_init_param.timeChrip = timeChrip
radar_init_param.timeChripGap = timeChripGap
radar_init_param.timeFrameGap = timeFrameGap
radar_init_param.numChannel = numChannel
radar_init_param.numRangeBin = numRangeBin
radar_init_param.numChrip = numChrip

print(f"雷达初始化参数:\n{radar_init_param}")
pyRadar.radardsp_init(radar_handle, radar_init_param)


# %% 输入所有帧

magSpec2D_list = []
magSpec2DRef_list = []

for frame in rdms_list:
    set_matrix3d_complex_int16(rdms, frame)
    pyRadar.radardsp_input_new_frame(radar_handle, rdms)

    magSpec2D_list.append(radar_handle.getMagSpec2D())
    magSpec2DRef_list.append(np.sum(np.abs(frame) * np.sqrt(2**25), axis=0))

mese = np.mean([np.mean(np.abs(magSpec2D_list[i] - magSpec2DRef_list[i]) ** 2) for i in range(len(magSpec2D_list))])
print("幅度谱均方误差：", mese)
# %%
go.Figure(data=[go.Surface(z=magSpec2D_list[0])]).show()


# %%
dh.draw_2d_spectrumlist(magSpec2D_list[::100], title="幅度谱").show()
dh.draw_2d_spectrumlist(magSpec2DRef_list[::100], title="幅度谱").show()

# %%

#%%
import sys
import os
from  pyRadar import pyRadar
import numpy as np


#%%

radarHandle = pyRadar.RadarHandle()
radarParam = radarHandle.param

print(f"type of radarParam.wavelength: {type(radarParam.wavelength)}")
print(f"type of radarParam.numPoint :{type(radarParam.numPoint)}")

#%%
pyRadar.radardsp_init(radarHandle)

print(f"radarHandle.param.numChannel: {radarHandle.param.numChannel}")
print(f"radarHandle.param.numRangeBin: {radarHandle.param.numRangeBin}")
print(f"radarHandle.param.numChrip: {radarHandle.param.numChrip}")


#%%
rdms = (np.arange(2*25*32)-1j*np.arange(2*25*32)).reshape(2,25,32).astype(np.complex64)
pyRadar.radardsp_input_new_frame(radarHandle,rdms)

print(radarHandle.basic.staticClutter)
#%%
print(radarHandle.basic.staticClutter)
#%%

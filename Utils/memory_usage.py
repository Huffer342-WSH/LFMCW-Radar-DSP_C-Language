# %%
import numpy as np

numChannel = 2
numChrip = 64
numRangeBin = 30
numSample = 256
# %%

m = dict()

m["RawMean"] = 2 * numChannel * numSample * 4
m["Raw"] = 8 * numChannel * numSample * 2
m["RangeFFT"] = numChannel * numSample * 2
m["VeloFFT"] = numChannel * numChrip * numRangeBin * 4

m["RDM"] = numChannel * numChrip * numRangeBin * 4
m["MagSpec2D"] = numChannel * numRangeBin * 4

m["MicroMove_prevFrame"] = numRangeBin * 2
m["MicroMove_deltaPhase"] = 40 * numRangeBin * 2

m["CFAR"] = 40 * 12
m["MEAS"] = 40 * 12

print(m)
sum = 0
for k, v in m.items():
    sum += v
print(sum)

# %%

# %%
from pylfmcwradar import pyradar_fixed as pyRadar


# %%
meas = pyRadar.radar_measurement_list_alloc(10)

# %%
meas.meas[0].distance = 1

print(f"meas.capacity: {meas.capacity}")
print(f"meas.num: {meas.num}")
for i in range(meas.num):
    measurement = meas.meas[i]
    measurement: pyRadar.measurements
    print(
        f"measurement.distance: {measurement.distance}\n"
        f"measurement.velocity: {measurement.velocity}\n"
        f"measurement.azimuth: {measurement.azimuth}\n"
        f"measurement.amp: {measurement.amp}\n"
        f"measurement.snr: {measurement.snr}\n"
    )


# %%
a = pyRadar.measurements()
m = pyRadar.measurements()
m.distance = 1
m.velocity = 2
m.azimuth = 3
m.amp = 4
m.snr = 5
meas.meas[0] = m
a = m

print(m.amp)
print(a.amp)
print(meas.meas[0].amp)
meas.num += 1

print(f"meas.capacity: {meas.capacity}")
print(f"meas.num: {meas.num}")
for i in range(meas.num):
    measurement = meas.meas[i]
    measurement: pyRadar.measurements
    print(
        f"measurement.distance: {measurement.distance}\n"
        f"measurement.velocity: {measurement.velocity}\n"
        f"measurement.azimuth: {measurement.azimuth}\n"
        f"measurement.amp: {measurement.amp}\n"
        f"measurement.snr: {measurement.snr}\n"
    )

# %%
print(f"meas.meas[0].amp: {meas.meas[0].amp}")

meas.meas[0].amp = 10

print(f"meas.meas[0].amp: {meas.meas[0].amp}")

# %%

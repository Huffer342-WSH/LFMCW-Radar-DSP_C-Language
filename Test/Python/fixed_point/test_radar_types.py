from pylfmcwradar import pyradar_fixed as pyRadar
import pytest

meas_list = pyRadar.radar_measurement_alloc(10)


def test_radar_measurement_list_initialization():
    # Test initialization with radar_measurement_alloc
    capacity = 5
    radar_list = pyRadar.radar_measurement_alloc(capacity)
    assert radar_list.num == 0
    assert radar_list.capacity == capacity


def test_radar_measurement_list_meas_property():
    # Allocate radar measurement list
    capacity = 5
    radar_list = pyRadar.radar_measurement_alloc(capacity)

    # Create a sample measurement
    measurement = pyRadar.measurements()
    measurement.distance = 100.0
    measurement.velocity = 50.0
    measurement.azimuth = 30.0
    measurement.amp = 1.5
    measurement.snr = 10.0

    # Set measurements using Python list
    radar_list.meas = [measurement] * capacity
    assert radar_list.num == capacity

    # Verify that the measurements were correctly assigned
    for i, meas in enumerate(radar_list.meas):
        assert meas.distance == 100.0
        assert meas.velocity == 50.0
        assert meas.azimuth == 30.0
        assert meas.amp == 1.5
        assert meas.snr == 10.0

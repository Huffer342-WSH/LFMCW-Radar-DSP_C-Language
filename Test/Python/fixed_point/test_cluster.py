# %%
import pytest
import numpy as np
from pylfmcwradar import pyradar_fixed as pyRadar

from sklearn.datasets import make_blobs
from sklearn.preprocessing import StandardScaler
import plotly.graph_objects as go


def draw(X, labels, title="DBSCAN"):
    n_clusters_ = len(set(labels)) - (1 if -1 in labels else 0)
    unique_labels = set(labels)
    core_samples_mask = np.zeros_like(labels, dtype=bool)

    figdata = []
    for k in unique_labels:

        class_member_mask = labels == k

        xy = X[class_member_mask & ~core_samples_mask]
        figdata.append(
            go.Scatter(
                x=xy[:, 0],
                y=xy[:, 1],
                mode="markers",
                marker=dict(size=5, color=k),
                name=str(k),
            )
        )
    return go.Figure(data=figdata, layout=go.Layout(title=f"{title} 簇的数量:{n_clusters_}"))


# %%


def test_dbscan():
    centers = [[3000, 3000], [-3000, -3000], [3000, -3000], [-3000, 3000]]
    X, labels_true = make_blobs(n_samples=300, centers=centers, cluster_std=350)
    min_samples = 2
    eps = 500
    meas = pyRadar.radar_measurement_alloc(len(X))
    meas.meas["distance"] = np.linalg.norm(X, axis=1).astype(np.int32)
    meas.meas["azimuth"] = (np.atan2(X[:, 1], X[:, 0]) * (1 << 13)).astype(np.int16)
    meas.meas["velocity"] = 0
    meas.num = len(X)

    _labels = pyRadar.radar_cluster_dbscan(meas, 1 << 16, 0, eps, min_samples)
    """ 使用scikit-learn中的DBSCAN算法 """
    from sklearn import metrics
    from sklearn.cluster import DBSCAN

    db = DBSCAN(eps=eps, min_samples=min_samples).fit(X)
    labels = db.labels_

    assert np.sum(labels != _labels) < len(X) * 0.01


# %%
if __name__ == "__main__":
    centers = [[3000, 3000], [-3000, -3000], [3000, -3000], [-3000, 3000], [0, 0]]

    X, labels_true = make_blobs(n_samples=len(centers) * 6, centers=centers, cluster_std=350)
    go.Figure(data=[go.Scatter(x=X[:, 0], y=X[:, 1], mode="markers")], layout=go.Layout(title="原始数据")).show()
    min_samples = 3
    eps = 500

    meas = pyRadar.radar_measurement_alloc(len(X))
    meas.meas["distance"] = np.linalg.norm(X, axis=1).astype(np.int32)
    meas.meas["azimuth"] = (np.atan2(X[:, 1], X[:, 0]) * (1 << 13)).astype(np.int16)
    meas.meas["velocity"] = 0
    meas.num = len(X)

    # C语言模块
    _labels = pyRadar.radar_cluster_dbscan(meas, 1 << 16, 0, eps, min_samples - 1)

    # scikit-learn
    from sklearn import metrics
    from sklearn.cluster import DBSCAN

    db = DBSCAN(eps=eps, min_samples=min_samples).fit(X)
    labels = db.labels_
    n_clusters_ = len(set(labels)) - (1 if -1 in labels else 0)
    n_noise_ = list(labels).count(-1)

    draw(X, labels).show()
    draw(X, _labels).show()
    print(f"不一致个数:{np.sum(labels != _labels) }")
# %%

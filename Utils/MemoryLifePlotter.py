import re
import plotly.graph_objects as go
import numpy as np


class MemoryLifePlotter:
    def __init__(self, logfile):
        self.logfile = logfile
        self.malloc_re = re.compile(r"MM:t=(\d+) alloc\s+p=\[(0x[0-9a-fA-F]+)\]\s+sz=(\d+) @ (.+)")
        self.free_re = re.compile(r"MM:t=(\d+) free\s+p=\[(0x[0-9a-fA-F]+)\] +@ (.+)")
        self.allocs = {}
        self.records = []
        self.prev_ts = np.uint32(10)
        self.timestamp = 0
        self.end_ts = 0
        self.addr_mapped = False  # 标记是否已映射地址

    def _calc_ts(self, ts):
        ts = np.uint32(ts)
        self.timestamp += ts - self.prev_ts
        self.prev_ts = ts
        return self.timestamp

    def parse_log(self):
        with open(self.logfile) as f:
            for line in f:
                m = None
                if m := self.malloc_re.match(line):
                    ts, ptr, size, loc = m.groups()
                    is_malloc = True
                elif m := self.free_re.match(line):
                    ts, ptr, loc = m.groups()
                    is_malloc = False
                else:
                    continue
                ts = np.uint32(int(ts))
                ptr = int(ptr, 16)
                ts_val = self._calc_ts(ts)
                self.end_ts = max(self.end_ts, ts_val)
                if is_malloc:
                    size = int(size)
                    self.allocs[ptr] = {"info": loc, "ptr": ptr, "start": ts_val, "end": None, "size": size}
                else:  # free操作
                    if ptr in self.allocs:
                        alloc = self.allocs.pop(ptr)
                        alloc["end"] = ts_val
                        self.records.append(alloc)
                    else:
                        print(f"Warning: free of unknown ptr {ptr} at {ts}")

        # 添加未释放块
        self.records.extend(self.allocs.values())
        self.end_ts += 1000

    def remap_addresses(self):
        """把稀疏的地址空间压缩映射到连续坐标系"""
        intervals = []
        for rec in self.records:
            start = rec["ptr"]
            end = rec["ptr"] + rec["size"]
            intervals.append((start, end))

        # 收集所有边界
        points = sorted(set([p for interval in intervals for p in interval]))

        # 构建映射: 原始地址 -> 压缩坐标
        addr_to_y = {}
        y = 0
        for i in range(len(points) - 1):
            addr_to_y[points[i]] = y
            gap = points[i + 1] - points[i]
            # 压缩间隙为1个单位，而不是实际gap，避免稀疏
            y += 1
        addr_to_y[points[-1]] = y

        # 应用映射
        for rec in self.records:
            rec["y0"] = addr_to_y[rec["ptr"]]
            rec["y1"] = addr_to_y[rec["ptr"] + rec["size"]]

        self.addr_mapped = True

    def plot(self, title="Memory Allocation Lifetime Map"):
        if not self.records:
            raise ValueError("No records to plot. Did you call parse_log()?")

        if not self.addr_mapped:
            self.remap_addresses()

        fig = go.Figure()

        for rec in self.records:
            x0 = rec["start"]
            x1 = rec["end"] if rec["end"] else self.end_ts
            y0 = rec["y0"]
            y1 = rec["y1"]
            pattern_shape = "" if rec["end"] else "/"

            fig.add_trace(
                go.Scatter(
                    x=[x0, x1, x1, x0, x0],
                    y=[y0, y0, y1, y1, y0],
                    fill="toself",
                    mode="none",
                    fillpattern=dict(shape=pattern_shape),
                    name=(
                        f"<b>{hex(rec['ptr'])}</b><br>"
                        f"Start: {x0}<br>"
                        f"End: {x1}<br>"
                        f"Size: {rec['size']} B<br>"
                        f"{rec['info']}<br>" + ("" if rec["end"] else "<span style='color:red'><b>未释放</b></span>")
                    ),
                    showlegend=True,
                )
            )

        fig.update_layout(
            title=title,
            xaxis_title="Time (ticks)",
            yaxis_title="Compressed Memory Address",
        )

        return fig


if __name__ == "__main__":
    plotter = MemoryLifePlotter("memlog.txt")
    plotter.parse_log()
    fig = plotter.plot()
    fig.show(renderer="browser")

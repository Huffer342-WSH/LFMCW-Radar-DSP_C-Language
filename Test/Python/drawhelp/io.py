# %%
__all__ = ["plotly_fig_to_video", "plotly_fig_to_video_multiprocess", "plotly_fig_to_video_joblib"]

import plotly.graph_objects as go
import plotly.io as pio
import ffmpeg


import tqdm

import __main__


def get_unique_filename(filepath):
    """
    检查文件是否存在，如果存在，给文件名加上后缀(1), (2)...直到找到一个不存在的文件名。
    """
    import os

    base, ext = os.path.splitext(filepath)
    counter = 1
    new_filepath = filepath
    while os.path.exists(new_filepath):
        new_filepath = f"{base}({counter}){ext}"
        counter += 1
    return new_filepath

import time


def __generate_frame(fig, frames, indices, queue):
    """
    生成单个帧的图像并将其通过队列传递给主进程。
    """
    for i, frame in zip(indices, frames):
        img_bytes = fig.update(data=frame.data).to_image(format="png")
        queue.put((i, img_bytes))
    return


def plotly_fig_to_video_multiprocess(fig: go.Figure, output_path: str, fps: int = 30, width: int = None, height: int = None, njobs: int = -1):
    """
    (弃用，请使用 plotly_fig_to_video_joblib)使用多进程生成图片并编码成视频，需要在 `if __name__ == "__main__":` 中调用


    Parameters
    ----------
        fig (go.Figure):
            需要保存成视频的plotly.graph_objects.Figure 对象
        output_path (str):
            视频保存路径
        fps (int, optional):
            帧率. Defaults to 30.
        width (int, optional):
            视频宽度. Defaults to None.
        height (int, optional):
            视频高度. Defaults to None.
        njobs (int, optional):
            线程数. Defaults to -1.

    Examples
    --------
    >>> import plotly.graph_objects as go
    >>> import numpy as np
    >>> from drawhelp.io import plotly_fig_to_video_multiprocess

    >>> frames = [go.Frame(data=[go.Scatter(x=np.arange(100), y=np.sin(np.arange(100) * np.pi / 30 + t))]) for t in range(100)]
    >>> fig = go.Figure(data=frames[0].data, frames=frames)

    >>> if __name__ == "__main__":
    >>>     plotly_fig_to_video_multiprocess(fig, "output_video.mp4", width=1080, height=600)

    """
    print("请使用plotly_fig_to_video_joblib")
    return

    import psutil
    from multiprocessing import Queue, Process
    from multiprocessing.shared_memory import SharedMemory
    from threading import Thread, Event
    import pickle
    import heapq

    def store_farme_to_heap(heap, queue, total_frames):
        count = 0
        while count < total_frames:
            frame_index, img_bytes = queue.get()
            heapq.heappush(heap, (frame_index, img_bytes))
            recv_image_event.set()
            count += 1

    if not getattr(__main__, "__file__", None):
        return

    output_path = get_unique_filename(output_path)

    if njobs <= 0:
        njobs = psutil.cpu_count(logical=False)

    print(f"Number of Processes:{njobs}")

    fig.layout.width = width or fig.layout.width or 1080
    fig.layout.height = height or fig.layout.height or 607
    print(f"Image size: {fig.layout.width}x{fig.layout.height}")

    # 获取所有frames
    frames = fig.frames
    num_frames = len(frames)
    fig_template = go.Figure(data=fig.data, layout=fig.layout)
    fig_template.update_layout(dict1=dict(updatemenus=[], sliders=[]), overwrite=True)

    heap = []
    image_queue = Queue()  # 创建一个队列用于传递数据
    recv_image_event = Event()

    fig_data = pickle.dumps(pio.to_json(fig))
    figure_shm = SharedMemory(create=True, size=len(fig_data))
    figure_shm.buf[:] = fig_data

    # 启动多个进程生成图像
    processList = []
    for i in range(njobs):
        print(f"Creating Process {i}... ", end="")
        frames = fig.frames[i:num_frames:njobs]
        p = Process(target=__generate_frame, args=(fig_template, frames, range(i, num_frames, njobs), image_queue))
        processList.append(p)
        p.start()
        print(f"{p.pid} started")

    # 启动一个线程将图像数据存储到堆
    theread_save_to_heap = Thread(target=store_farme_to_heap, args=(heap, image_queue, num_frames))
    theread_save_to_heap.start()

    # 使用ffmpeg进行视频编码，从pipe中读取图像
    process = (
        ffmpeg.input("pipe:0", framerate=fps, format="image2pipe", pix_fmt="yuv420p")  # 通过stdin传输图像
        .output(
            output_path,
            vcodec="h264_nvenc",
            cq=19,
        )
        .global_args("-loglevel", "warning")  # 设置日志级别为 quiet
        .run_async(pipe_stdin=True)  # 异步运行，打开stdin管道
    )

    # 按照编号顺序将图像数据传输给ffmpeg
    for i in tqdm.tqdm(range(num_frames)):
        while not heap or heap[0][0] != i:
            recv_image_event.wait()
        recv_image_event.clear()
        _, img_data = heapq.heappop(heap)
        process.stdin.write(img_data)
        del img_data

    # 关闭stdin，告诉ffmpeg输入已经完成
    process.stdin.close()

    for p in processList:
        p.join()
    theread_save_to_heap.join()
    # 清理共享内存
    figure_shm.close()
    figure_shm.unlink()
    process.wait()
    print(f"Video saved to {output_path}")


def plotly_fig_to_video(fig: go.Figure, output_path: str, fps: int = 30, width: int = None, height: int = None):

    output_path = get_unique_filename(output_path)

    fig.layout.width = width or fig.layout.width or 1080
    fig.layout.height = height or fig.layout.height or 607
    print(f"Image size: {fig.layout.width}x{fig.layout.height}")

    frames = fig.frames
    num_frames = len(frames)
    fig_template = go.Figure(data=fig.data, layout=fig.layout)
    fig_template.update_layout(dict1=dict(updatemenus=[], sliders=[]), overwrite=True)

    process = (
        ffmpeg.input("pipe:0", framerate=fps, format="image2pipe", pix_fmt="yuv420p")  # 通过stdin传输图像
        .output(
            output_path,
            vcodec="h264_nvenc",
            cq=19,
        )
        .global_args("-loglevel", "warning")  # 设置日志级别为 quiet
        .run_async(pipe_stdin=True)  # 异步运行，打开stdin管道
    )

    for i in tqdm.tqdm(range(num_frames)):
        fig_template.update(data=fig.frames[i].data)
        img_data = fig_template.to_image(format="png")
        process.stdin.write(img_data)

    # 关闭stdin，告诉ffmpeg输入已经完成
    process.stdin.close()

    process.wait()
    print(f"Video saved to {output_path}")


def __frame_to_image__for_plotly_fig_to_video_joblib(fig, frame):
    fig.update(data=frame.data)
    img_data = fig.to_image(format="png")
    return img_data


def plotly_fig_to_video_joblib(fig: go.Figure, output_path: str, fps: int = 30, width: int = None, height: int = None):
    """将plotly.graph_objects.Figure 对象编码成视频

    Parameters
    ----------
    fig : go.Figure
        待编码的 plotly.graph_objects.Figure 对象，带有 frames 属性
    output_path : str
        视频保存路径
    fps : int, optional
        帧数, by default 30
    width : int, optional
        宽度，会覆盖fig.layout.width，假如都没有设置默认为1080
    height : int, optional
        高度，会覆盖fig.layout.height，假如都没有设置默认为607
    """
    import joblib

    if not getattr(__main__, "__file__", None):
        return

    output_path = get_unique_filename(output_path)

    fig.layout.width = width or fig.layout.width or 1080
    fig.layout.height = height or fig.layout.height or 607
    print(f"Image size: {fig.layout.width}x{fig.layout.height}")

    frames = fig.frames
    num_frames = len(frames)
    fig_template = go.Figure(data=fig.data, layout=fig.layout)
    fig_template.update_layout(dict1=dict(updatemenus=[], sliders=[]), overwrite=True)

    # 创建 ffmpeg 进程
    process = (
        ffmpeg.input("pipe:0", framerate=fps, format="image2pipe", pix_fmt="yuv420p")
        .output(output_path, vcodec="h264_nvenc", cq=19)
        .global_args("-loglevel", "warning")
        .run_async(pipe_stdin=True)
    )

    # 使用 joblib 并行化生成图像帧
    frame_images = joblib.Parallel(n_jobs=-1, backend="multiprocessing")(
        joblib.delayed(__frame_to_image__for_plotly_fig_to_video_joblib)(fig_template, frames[i]) for i in tqdm.trange(num_frames, desc="Generating Images")
    )

    # 按照顺序写入所有帧数据到 ffmpeg
    for img_data in tqdm.tqdm(frame_images, desc="Encoding Video   "):
        # Start the ffmpeg process for video encoding
        process.stdin.write(img_data)

    # 关闭stdin，告诉ffmpeg输入已经完成
    process.stdin.close()
    process.wait()

    print(f"Video saved to {output_path}")

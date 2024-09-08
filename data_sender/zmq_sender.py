import zmq
import time
import json
import random

# 创建一个 ZMQ 上下文
context = zmq.Context()

# 创建一个 ZMQ 发布者套接字
socket = context.socket(zmq.PUB)
socket.bind("tcp://*:5555")  # 绑定到端口

while True:
    # 生成随机的 line 数据
    data = {
        'line': [{'x': i, 'y': random.random() * 100} for i in range(10)],
        'constellation': [{'real': random.random() * 100, 'imag': random.random() * 100} for _ in range(10)]
    }

    # 将数据序列化为 JSON 格式
    message = json.dumps(data)

    # 向 ZMQ 发布消息
    socket.send_string(message)

    print(f"已发送数据: {message}")

    # 每隔 1 秒发送一次数据
    time.sleep(1)

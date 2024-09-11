const WebSocket = require('ws');
const protobuf = require('protobufjs');

// 创建 WebSocket 服务器
const wss = new WebSocket.Server({ port: 9000 });
const frontendClients = []; // 保存前端连接的客户端
// 加载 .proto 文件
protobuf.load("../data_sender/result.proto", (err, root) => {
    if (err) throw err;

    // 获取 `result` 消息类型
    const ResultMessage = root.lookupType("result");

    // 监听 WebSocket 连接
    wss.on('connection', (ws, req) => {
        const path = req.url; // 获取连接的路径
        console.log(`Client connected with path: ${path}`);
        if (path === '/frontend') {
            // 如果是来自前端的连接，保存到 `frontendClients`
            frontendClients.push(ws);
            console.log('Frontend client connected');

            // 当连接关闭时，从 `frontendClients` 中移除
            ws.on('close', () => {
                const index = frontendClients.indexOf(ws);
                if (index !== -1) {
                    frontendClients.splice(index, 1);
                }
                console.log('Frontend client disconnected');
            });

        }
        else {
            // 处理接收到的消息
            ws.on('message', (message) => {
                try {
                    // 假设消息是二进制的 Protobuf 数据
                    const decodedMessage = ResultMessage.decode(message);
                    console.log("recv length: " + message.length)
                    // 打印反序列化后的对象
                    // console.log("Received Protobuf message:", JSON.stringify(decodedMessage, null, 2));

                    // 将反序列化后的 Protobuf 数据作为 JSON 发送给所有前端连接的客户端
                    const jsonMessage = JSON.stringify(decodedMessage);
                    frontendClients.forEach(client => {
                        if (client.readyState === WebSocket.OPEN) {
                            client.send(jsonMessage);
                        }
                    });

                    // 这里可以对反序列化后的数据进行进一步的处理
                } catch (e) {
                    console.error("Failed to decode message:", e);
                }
            });

            // 处理连接关闭
            ws.on('close', () => {
                console.log('Client disconnected');
            });
        }
    });
});

console.log('WebSocket server is running on ws://localhost:9000');

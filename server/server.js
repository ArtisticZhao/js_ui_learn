const WebSocket = require('ws');
const zmq = require('zeromq');

// 创建 WebSocket 服务器
const wss = new WebSocket.Server({ port: 9000 });

// 存储所有连接的客户端
let clients = [];

wss.on('connection', (ws) => {
    console.log('前端连接成功');
    clients.push(ws);

    ws.on('close', () => {
        clients = clients.filter(client => client !== ws);
    });
});

// 启动 ZMQ 订阅者并转发消息到 WebSocket 客户端
async function startZMQSubscriber() {
    const zmqSocket = new zmq.Subscriber();
    zmqSocket.connect('tcp://localhost:5555');
    zmqSocket.subscribe('');

    for await (const [message] of zmqSocket) {
        console.log('接收到 ZMQ 数据: ', message.toString());

        // 将 ZMQ 数据转发给所有前端客户端
        clients.forEach(client => {
            if (client.readyState === WebSocket.OPEN) {
                client.send(message.toString());
            }
        });
    }
}

startZMQSubscriber().catch(console.error);

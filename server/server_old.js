const WebSocket = require('ws');

// 创建 WebSocket 服务器，监听 9000 端口
const wss = new WebSocket.Server({ port: 9000 });

// 当有客户端连接时触发
wss.on('connection', ws => {
  console.log('Client connected');

  // 定期发送 JSON 数据给客户端
  const interval = setInterval(() => {
    const data = {
      line: Array.from({ length: 10 }, (_, i) => ({
        x: i,
        y: Math.random() * 100 // 生成随机数据
      }))
    };
    ws.send(JSON.stringify(data));
  }, 1000); // 每秒发送一次数据

  // 当客户端断开连接时触发
  ws.on('close', () => {
    clearInterval(interval);
    console.log('Client disconnected');
  });
});

console.log('WebSocket server is running on ws://localhost:9000');

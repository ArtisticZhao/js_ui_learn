#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

typedef websocketpp::client<websocketpp::config::asio_client> client;

class WebSocketClient {
public:
    // 构造函数，传入 WebSocket 服务器地址
    WebSocketClient(const std::string& uri)
        : m_uri(uri), m_is_connected(false) {

        m_client.clear_access_channels(websocketpp::log::alevel::all);  // 关闭所有详细日志
        m_client.set_access_channels(websocketpp::log::alevel::connect | websocketpp::log::alevel::disconnect | websocketpp::log::alevel::app);  // 仅保留连接、断开和应用日志
        // 初始化 WebSocket++ 的 Asio 支持
        m_client.init_asio();

        // 设置连接关闭处理程序// 设置 on_open 处理程序，连接建立后会调用这个函数
        m_client.set_open_handler([this](websocketpp::connection_hdl hdl) {
            m_hdl = hdl;
            m_is_connected = true;
            std::cout << "Connection opened." << std::endl;
        });
        m_client.set_close_handler([this](websocketpp::connection_hdl hdl) {
            std::cout << "Connection closed." << std::endl;
            m_is_connected = false;
        });

        // 设置错误处理程序
        m_client.set_fail_handler([this](websocketpp::connection_hdl hdl) {
            std::cout << "Connection failed." << std::endl;
            m_is_connected = false;
        });
    }

    // 连接到 WebSocket 服务器
    bool connect() {
        websocketpp::lib::error_code ec;
        client::connection_ptr con = m_client.get_connection(m_uri, ec);

        if (ec) {
            std::cout << "Could not create connection because: " << ec.message() << std::endl;
            return false;
        }
        // 连接服务器
        m_client.connect(con);

        // 启动 WebSocket++ 的 IO 线程
        m_thread = std::thread([this]() { m_client.run(); });

        // 等待连接完成
        std::this_thread::sleep_for(std::chrono::seconds(1));

        m_is_connected = true;
        std::cout << "Connected to " << m_uri << std::endl;
        return true;
    }

    // 发送数据
    bool send(const std::string& message) {
        if (!m_is_connected) {
            std::cout << "Not connected to the server." << std::endl;
            return false;
        }

        websocketpp::lib::error_code ec;
        m_client.send(m_hdl, message, websocketpp::frame::opcode::binary, ec);

        if (ec) {
            std::cout << "Send failed because: " << ec.message() << std::endl;
            return false;
        }

        std::cout << "Data sent successfully." << std::endl;
        return true;
    }

    // 断开连接
    void close() {
        if (m_is_connected) {
            m_client.close(m_hdl, websocketpp::close::status::normal, "Normal closure");
            m_thread.join();  // 等待线程结束
        }
    }

    ~WebSocketClient() {
        close();  // 确保析构时断开连接
    }

private:
    client m_client;                      // WebSocket++ 客户端对象
    std::string m_uri;                    // WebSocket 服务器 URI
    websocketpp::connection_hdl m_hdl;    // WebSocket 连接句柄
    std::thread m_thread;                 // 运行 IO 服务的线程
    bool m_is_connected;                  // 连接状态标志
};

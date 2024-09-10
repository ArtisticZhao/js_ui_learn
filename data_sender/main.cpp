#include <iostream>
#include <stdint.h>
#include "result.pb.h"
#include "wsclient.hpp"

int main (int argc, char *argv[]) {
    result res;
    for(int32_t i=0; i<50; ++i) {
        res.add_spectrum(0.00);
    }
    for(int32_t i=0; i<100; ++i) {
        res.add_spectrum(1.00);
    }
    for(int32_t i=0; i<50; ++i) {
        res.add_spectrum(0.00);
    }
    // 创建 EVM 消息并设置数据
    EVM* evm_entry = res.add_evm_res();
    evm_entry->set_field("All");
    evm_entry->set_evm_rms(1.5);
    evm_entry->set_evm_max(2.5);
    evm_entry->set_evm_min(0.5);

    evm_entry = res.add_evm_res();
    evm_entry->set_field("pdsch");
    evm_entry->set_evm_rms(0.5);
    evm_entry->set_evm_max(1.5);
    evm_entry->set_evm_min(0.2);

    // 添加 evm_vs_symbol 数据
    res.add_evm_vs_symbol(0.12);
    res.add_evm_vs_symbol(0.15);

    // 添加 evm_vs_rb 数据
    res.add_evm_vs_rb(0.25);
    res.add_evm_vs_rb(0.35);

 // 创建 constellation 消息并设置数据
    constellation* const_entry = res.add_constellation_data();
    const_entry->set_field("Constellation Field 1");

    // 设置 constellation 的 real, imag, ref_real, ref_imag 列表
    const_entry->add_real(1.1);
    const_entry->add_real(1.2);
    const_entry->add_imag(0.5);
    const_entry->add_imag(0.6);
    const_entry->add_ref_real(1.0);
    const_entry->add_ref_real(1.1);
    const_entry->add_ref_imag(0.5);
    const_entry->add_ref_imag(0.55);

     // 使用 SerializeAsString 序列化
    std::string serialized_data = res.SerializeAsString();
    if (!serialized_data.empty()) {
        std::cout << "Serialized data successfully!" << std::endl;
    } else {
        std::cerr << "Failed to serialize data!" << std::endl;
    }
    std::cout << serialized_data;

    // 创建 WebSocketClient 实例，连接到 WebSocket 服务器
    WebSocketClient ws_client("ws://localhost:9000");

    // 连接 WebSocket 服务器
    if (ws_client.connect()) {
        // 发送 Protobuf 序列化后的数据
        std::this_thread::sleep_for(std::chrono::seconds(1));
        ws_client.send(serialized_data);

        // 停留几秒等待服务器响应或其他任务
        std::this_thread::sleep_for(std::chrono::seconds(5));

        // 关闭连接
        ws_client.close();
    }
    return 0;
}

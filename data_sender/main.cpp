#include <chrono>
#include <iostream>
#include <stdint.h>
#include <vector>
#include <random>
#include "result.pb.h"
#include "wsclient.hpp"


typedef std::vector<float> std_fvec;

std_fvec gen_random(int32_t N, float min, float max) {
    // 使用随机设备和梅森旋转器引擎生成随机数
    std::random_device rd;  // 获取随机种子
    std::mt19937 gen(rd()); // 梅森旋转算法的随机数引擎
    std::uniform_real_distribution<float> dist(min, max); // 设置浮点数的范围

    std::vector<float> randomFloats; // 用于存储生成的随机数
    randomFloats.reserve(N); // 预先分配 N 个元素的空间

    // 生成 N 个随机数并存储到 vector 中
    for (int i = 0; i < N; ++i) {
        randomFloats.push_back(dist(gen));
    }
    return randomFloats;
}

int main (int argc, char *argv[]) {
    // 创建 WebSocketClient 实例，连接到 WebSocket 服务器
    WebSocketClient ws_client("ws://localhost:9000");
    if (!ws_client.connect()) return -1;
    for(int32_t time=0; time<10; ++time) {

        result res;
        for(int32_t i=0; i<50; ++i) {
            res.add_spectrum(0.00);
        }
        std_fvec sp = gen_random(100, 0.8, 1.2);
        for(int32_t i=0; i<sp.size(); ++i) {
            res.add_spectrum(sp[i]);
        }
        for(int32_t i=0; i<50; ++i) {
            res.add_spectrum(0.00);
        }
        // 创建 EVM 消息并设置数据
        std_fvec evm0 = gen_random(2, 0, 3);
        EVM* evm_entry = res.add_evm_res();
        evm_entry->set_field("All");
        evm_entry->set_evm_rms((evm0[0]+evm0[1])/2.0f);
        evm_entry->set_evm_max(std::max(evm0[0], evm0[1]));
        evm_entry->set_evm_min(std::min(evm0[0], evm0[1]));

        std_fvec evm1 = gen_random(2, 0, 3);
        evm_entry = res.add_evm_res();
        evm_entry->set_field("pdsch");
        evm_entry->set_evm_rms((evm1[0]+evm1[1])/2.0f);
        evm_entry->set_evm_max(std::max(evm1[0], evm1[1]));
        evm_entry->set_evm_min(std::min(evm1[0], evm1[1]));

        // 添加 evm_vs_symbol 数据
        std_fvec evm_sym = gen_random(1000, 0.3, 3.5);
        for(auto e:evm_sym) res.add_evm_vs_symbol(e);

        // 添加 evm_vs_rb 数据
        std_fvec evm_rb = gen_random(273, 0.3, 3.5);
        for(auto e:evm_rb) res.add_evm_vs_rb(e);

     // 创建 constellation 消息并设置数据
        constellation* const_entry = res.add_constellation_data();
        const_entry->set_field("Constellation Field 1");

        // 设置 constellation 的 real, imag, ref_real, ref_imag 列表
        size_t c_size = 273*12*500;
        std_fvec sign = gen_random(c_size, -1, +1);
        std_fvec signQ = gen_random(c_size, -1, +1);
        std_fvec noise = gen_random(c_size, -0.003, +0.003);
        std_fvec noiseQ = gen_random(c_size, -0.003, +0.003);
        for(size_t i=0; i<sign.size(); ++i){
            if(sign[i] > 0) {
                const_entry->add_real(0.707 + noise[i]);
            }
            else {
                const_entry->add_real(-0.707 + noise[i]);
            }
            if(signQ[i] > 0) {
                const_entry->add_imag(0.707 + noiseQ[i]);
            }
            else {
                const_entry->add_imag(-0.707 + noiseQ[i]);
            }
        }
        const_entry->add_ref_real(0.707);
        const_entry->add_ref_imag(0.707);
        const_entry->add_ref_real(0.707);
        const_entry->add_ref_imag(-0.707);
        const_entry->add_ref_real(-0.707);
        const_entry->add_ref_imag(-0.707);
        const_entry->add_ref_real(-0.707);
        const_entry->add_ref_imag(0.707);

         // 使用 SerializeAsString 序列化
        std::string serialized_data = res.SerializeAsString();
        if (!serialized_data.empty()) {
            std::cout << "Serialized data successfully!" << std::endl;
        } else {
            std::cerr << "Failed to serialize data!" << std::endl;
        }

        // 连接 WebSocket 服务器
        ws_client.send(serialized_data);

        // 停留几秒等待服务器响应或其他任务
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

    }
    // 关闭连接
    ws_client.close();
    return 0;
}

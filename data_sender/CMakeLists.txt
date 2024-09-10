cmake_minimum_required(VERSION 3.5)
project(ProtobufCppExample)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find required protobuf package
find_package(protobuf CONFIG REQUIRED)
find_package(websocketpp REQUIRED)

# protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

# 添加可执行文件（假设 main.cpp 是你的入口源代码）
add_executable(ProtobufCppExample main.cpp result.pb.cc result.pb.h)

# 链接 Protobuf 库
target_link_libraries(ProtobufCppExample PRIVATE protobuf::libprotobuf)

add_library(Websockets INTERFACE)
target_include_directories(Websockets INTERFACE ${websocketpp_SOURCE_DIR}/websocketpp/websocketpp)
target_link_libraries(ProtobufCppExample PRIVATE Websockets)
# 包含 Protobuf 头文件路径
target_include_directories(ProtobufCppExample PRIVATE ${Protobuf_INCLUDE_DIRS})

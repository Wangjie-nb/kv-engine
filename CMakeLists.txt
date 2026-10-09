cmake_minimum_required(VERSION 3.10)
project(kvengine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -pthread")

# 头文件路径
include_directories(${PROJECT_SOURCE_DIR}/src)

# 输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 核心引擎源文件
set(CORE_SOURCES
    src/core/skiplist.cpp
    src/core/kvstore.cpp
    src/core/lru_cache.cpp
    src/core/wal.cpp
)

# 网络层源文件
set(NETWORK_SOURCES
    src/network/tcp_server.cpp
    src/network/cmd_parser.cpp
)

# 服务器程序
add_executable(kv_server
    ${CORE_SOURCES}
    ${NETWORK_SOURCES}
    src/main.cpp
)

# 压测程序
add_executable(bench
    ${CORE_SOURCES}
    tests/bench/benchmark.cpp
)

target_link_libraries(kv_server pthread)
target_link_libraries(bench pthread)

# KV Engine

基于跳表的高性能键值存储引擎，类似简化版Redis/LSM存储，支持WAL预写日志崩溃恢复、LRU热点缓存、epoll事件驱动网络服务。

## 项目亮点
- 内存索引采用跳表实现，插入/查找/删除平均时间复杂度O(logN)
- LRU热点缓存加速，哈希表+双向链表，O(1)时间复杂度定位
- WAL预写日志机制，写操作先落盘再更新内存，程序崩溃重启后通过回放日志恢复全部数据
- epoll事件驱动网络模型，单线程处理多客户端连接，解决TCP粘包半包问题
- 核心引擎加互斥锁，保证多线程并发读写数据一致性
- 模块化分层设计：核心引擎、网络层、工具库解耦，CMake构建

## 技术栈
C++17 / Linux epoll / 多线程 / 跳表 / LRU Cache / WAL预写日志 / CMake

## 性能表现（单机测试）
| 操作 | QPS |
|------|-----|
| 纯内存写入 | ~1,000,000 ops/s |
| WAL fsync写入 | ~200,000 ops/s |
| 读取（LRU命中） | ~1,200,000 ops/s |

> WAL开启fsync后写入性能下降约80%，换取崩溃后数据不丢失（持久性与性能的权衡）。

## 目录结构
```
kv-engine/
├── src/
│   ├── core/                # 核心引擎
│   │   ├── skiplist.h/cpp  # 跳表内存索引
│   │   ├── lru_cache.h/cpp # LRU热点缓存
│   │   ├── kvstore.h/cpp   # KV门面层
│   │   └── wal.h/cpp        # WAL预写日志
│   ├── network/             # 网络层
│   │   ├── tcp_server.h/cpp # epoll TCP服务器
│   │   └── cmd_parser.h/cpp # 命令解析
│   ├── util/                # 工具库
│   │   ├── status.h         # 统一错误码
│   │   ├── random.h         # 随机数工具
│   │   └── logger.h        # 日志工具
│   └── main.cpp             # 程序入口
├── tests/
│   └── bench/
│       └── benchmark.cpp    # 性能压测
├── CMakeLists.txt
└── README.md
```

## 编译运行
```bash
mkdir build && cd build
cmake ..
make
```

### 启动服务
```bash
./bin/kv_server
```
监听8888端口，telnet连接测试：
```bash
telnet 127.0.0.1 8888
```

### 支持的命令
```
set key value    # 写入键值对
get key          # 查询键值
del key          # 删除键
exists key       # 判断键是否存在
size             # 返回当前键的数量
clear            # 清空所有数据
```

### 性能压测
```bash
./bin/bench
```

## 设计思路
1. **为什么用跳表不用红黑树？**
   实现简单，范围查询方便，并发友好，Redis有序集合和LevelDB都采用跳表。

2. **WAL为什么先写日志再改内存？**
   保证程序崩溃时日志已经落盘，重启回放日志可以恢复全部数据，防止数据丢失。

3. **LRU为什么用哈希表+双向链表？**
   哈希表O(1)定位节点，双向链表O(1)移动/删除节点，整体O(1)时间复杂度。

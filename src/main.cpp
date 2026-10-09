#include "core/kvstore.h"
#include "core/wal.h"
#include "network/tcp_server.h"
#include "util/logger.h"
#include <iostream>

int main() {
    Logger::info("KV Engine starting...");

    KVStore* kv = new KVStore(1000);

    WAL* wal = new WAL("wal.log");
    kv->bindWAL(wal);

    Logger::info("Recovering from WAL...");
    wal->recover(kv->getSkipList());
    Logger::info("Recover done. Current size: " + std::to_string(kv->size()));

    TcpServer* server = new TcpServer(8888, kv);
    server->start();

    delete server;
    delete kv;
    delete wal;
    return 0;
}

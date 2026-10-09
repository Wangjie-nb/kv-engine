#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <string>
#include <map>
#include "core/kvstore.h"
#include "cmd_parser.h"

struct ClientContext {
    int fd;
    std::string recvBuf;
};

class TcpServer {
private:
    int port;
    bool running;
    KVStore* kv;
    int server_fd;
    int epoll_fd;
    std::map<int, ClientContext*> clients;
    CmdParser* parser;

    void acceptClient();
    void handleRead(int fd);
    void closeClient(int fd);

public:
    TcpServer(int p, KVStore* kv_ptr);
    ~TcpServer();

    bool start();
    void stop();
};

#endif

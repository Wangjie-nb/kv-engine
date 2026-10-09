#include "tcp_server.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <sys/epoll.h>
#include <fcntl.h>

static void setNonBlock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

TcpServer::TcpServer(int p, KVStore* kv_ptr)
    : port(p), running(false), kv(kv_ptr), server_fd(-1), epoll_fd(-1) {
    parser = new CmdParser(kv);
}

TcpServer::~TcpServer() {
    stop();
    delete parser;
}

bool TcpServer::start() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0) {
        std::cerr << "socket create error" << std::endl;
        return false;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if(bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "bind error" << std::endl;
        close(server_fd);
        return false;
    }

    if(listen(server_fd, 5) < 0) {
        std::cerr << "listen error" << std::endl;
        close(server_fd);
        return false;
    }

    epoll_fd = epoll_create1(0);
    if(epoll_fd < 0) {
        std::cerr << "epoll_create error" << std::endl;
        close(server_fd);
        return false;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);
    setNonBlock(server_fd);

    running = true;
    std::cout << "KV Server start on port " << port << std::endl;

    struct epoll_event events[1024];

    while(running) {
        int nready = epoll_wait(epoll_fd, events, 1024, -1);
        if(nready < 0) continue;

        for(int i = 0; i < nready; i++) {
            int fd = events[i].data.fd;

            if(fd == server_fd) {
                acceptClient();
            } else if(events[i].events & EPOLLIN) {
                handleRead(fd);
            } else if(events[i].events & (EPOLLHUP | EPOLLERR)) {
                closeClient(fd);
            }
        }
    }

    close(epoll_fd);
    close(server_fd);
    return true;
}

void TcpServer::acceptClient() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if(client_fd < 0) return;

    setNonBlock(client_fd);

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = client_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);

    ClientContext* ctx = new ClientContext();
    ctx->fd = client_fd;
    clients[client_fd] = ctx;

    std::string welcome = "KV Server ready. Commands: set k v | get k | del k | exists k | size | clear\n> ";
    send(client_fd, welcome.c_str(), welcome.size(), 0);
}

void TcpServer::handleRead(int fd) {
    ClientContext* ctx = clients[fd];
    if(ctx == nullptr) return;

    char buffer[4096];
    while(true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t n = read(fd, buffer, sizeof(buffer));
        if(n > 0) {
            ctx->recvBuf.append(buffer, n);
        } else if(n == 0) {
            closeClient(fd);
            return;
        } else {
            break;
        }
    }

    std::string& buf = ctx->recvBuf;
    size_t pos;
    while((pos = buf.find('\n')) != std::string::npos) {
        std::string line = buf.substr(0, pos);
        buf = buf.substr(pos + 1);

        if(!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if(line.empty()) {
            send(fd, "> ", 2, 0);
            continue;
        }

        std::string resp = parser->handleLine(line);
        send(fd, resp.c_str(), resp.size(), 0);
    }
}

void TcpServer::closeClient(int fd) {
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
    close(fd);

    auto it = clients.find(fd);
    if(it != clients.end()) {
        delete it->second;
        clients.erase(it);
    }
}

void TcpServer::stop() {
    running = false;
    for(auto& pair : clients) {
        close(pair.first);
        delete pair.second;
    }
    clients.clear();

    if(server_fd > 0) {
        close(server_fd);
        server_fd = -1;
    }
}

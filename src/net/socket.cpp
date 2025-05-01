#include "net/socket.h"

#include <asm-generic/socket.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "logger/logging.h"
#include "net/address.h"

cai::socket::~socket() {
    ::close(sockfd_);  // 关闭套接字，释放资源
}

void cai::socket::bind_address(const cai::address &localaddr) {
    if (0 != ::bind(sockfd_, (sockaddr *)localaddr.sock_addr(),
                    sizeof(sockaddr_in))) {
        LOG_FATAL << "bind sockfd: " << sockfd_ << " fail\n";
    }
}

void cai::socket::listen() {
    if (0 !=
        ::listen(sockfd_, 1024)) {  // 1024: 最大等待连接队列长度（backlog）
        LOG_FATAL << "listen sockfd: " << sockfd_ << " fail\n";
    }
}

int cai::socket::accept(cai::address *peeraddr) {
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    ::memset(&addr, 0, sizeof(addr));

    // 使用 accept4() 系统调用（支持原子设置标志）：
    // SOCK_NONBLOCK: 设置新连接为非阻塞模式
    // SOCK_CLOEXEC: 设置文件描述符在 exec() 时自动关闭
    int connfd = ::accept4(sockfd_, (sockaddr *)&addr, &len,
                           SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd >= 0) {
        peeraddr->set_sock_addr(addr);
    }
    return connfd;
}

void cai::socket::shutdown_write() {
    if (::shutdown(sockfd_, SHUT_WR) <
        0) {  // 调用 shutdown() 系统调用，SHUT_WR 表示禁止后续写操作
        LOG_ERROR << "shutdown write error\n";
    }
}

void cai::socket::set_tcp_no_delay(bool on) {
    int optval = on ? 1 : 0;
    // SOL_TCP: 表示设置 TCP 层选项
    // TCP_NODELAY: 禁用 Nagle 算法，降低延迟
    if (::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval,
                     sizeof(optval)) < 0) {
        LOG_ERROR << "set_tcp_no_delay fail";
    }
}
void cai::socket::set_reuse_addr(bool on) {
    int optval = on ? 1 : 0;
    // SOL_SOCKET: 表示设置通用套接字选项
    // SO_REUSEADDR: 允许绑定处于 TIME_WAIT 状态的地址
    if (::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval,
                     sizeof(optval)) < 0) {
        LOG_ERROR << "set_reuse_addr fail";
    }
}
void cai::socket::set_reuse_port(bool on) {
    int optval = on ? 1 : 0;
    // SO_REUSEPORT: 允许多个套接字绑定同一端口（需要内核支持）
    if (::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval,
                     sizeof(optval)) < 0) {
        LOG_ERROR << "set_reuse_port fail";
    }
}
void cai::socket::set_keep_alive(bool on) {
    int optval = on ? 1 : 0;
    // SO_KEEPALIVE: 定期检测死连接
    if (::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval,
                     sizeof(optval)) < 0) {
        LOG_ERROR << "set_keep_alive fail";
    }
}
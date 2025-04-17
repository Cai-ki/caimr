#include "caimr/net/socket.h"

#include <asm-generic/socket.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "caimr/logger/logging.h"
#include "caimr/net/address.h"

cai::socket::~socket() { ::close(sockfd_); }

void cai::socket::bind_address(const cai::address &localaddr) {
    if (0 != ::bind(sockfd_, (sockaddr *)localaddr.sock_addr(),
                    sizeof(sockaddr_in))) {
        LOG_FATAL << "bind sockfd: " << sockfd_ << " fail\n";
    }
}

void cai::socket::listen() {
    if (0 != ::listen(sockfd_, 1024)) {
        LOG_FATAL << "listen sockfd: " << sockfd_ << " fail\n";
    }
}

int cai::socket::accept(cai::address *peeraddr) {
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    ::memset(&addr, 0, sizeof(addr));

    int connfd = ::accept4(sockfd_, (sockaddr *)&addr, &len,
                           SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd >= 0) {
        peeraddr->set_sock_addr(addr);
    }
    return connfd;
}

void cai::socket::shutdown_write() {
    if (::shutdown(sockfd_, SHUT_WR) < 0) {
        LOG_ERROR << "shutdown write error\n";
    }
}

void cai::socket::set_tcp_no_delay(bool on) {
    int optval = on ? 1 : 0;
    if (::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval,
                     sizeof(optval)) < 0) {
        LOG_ERROR << "set_tcp_no_delay fail";
    }
}
void cai::socket::set_reuse_addr(bool on) {
    int optval = on ? 1 : 0;
    if (::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval,
                     sizeof(optval)) < 0) {
        LOG_ERROR << "set_reuse_addr fail";
    }
}
void cai::socket::set_reuse_port(bool on) {
    int optval = on ? 1 : 0;
    if (::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval,
                     sizeof(optval)) < 0) {
        LOG_ERROR << "set_reuse_port fail";
    }
}
void cai::socket::set_keep_alive(bool on) {
    int optval = on ? 1 : 0;
    if (::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval,
                     sizeof(optval)) < 0) {
        LOG_ERROR << "set_keep_alive fail";
    }
}
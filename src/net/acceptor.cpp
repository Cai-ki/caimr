#include "net/acceptor.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>

#include "log/logging.h"
#include "net/address.h"

namespace cai {
static int create_nonblocking() {
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                          IPPROTO_TCP);
    if (sockfd < 0) {
        LOG_FATALF("listen socket create error: %d", errno);
    }
    return sockfd;
}

acceptor::acceptor(eloop* loop, const address& listen_addr, bool reuse_port)
    : loop_(loop),
      accept_socket_(create_nonblocking()),
      accept_ch_(loop, accept_socket_.fd()),
      listening_(false) {
    accept_socket_.set_reuse_addr(true);
    accept_socket_.set_reuse_port(reuse_port);
    accept_socket_.bind_address(listen_addr);

    accept_ch_.set_read_callback(std::bind(&acceptor::handle_read, this));
}

acceptor::~acceptor() {
    accept_ch_.disable_all();
    accept_ch_.remove();
}

void acceptor::listen() {
    listening_ = true;
    accept_socket_.listen();
    accept_ch_.enable_reading();
}

void acceptor::handle_read() {
    address peer_addr;
    int connfd = accept_socket_.accept(&peer_addr);
    if (connfd >= 0) {
        if (new_conn_callback_) {
            new_conn_callback_(connfd, peer_addr);
        } else {
            ::close(connfd);
        }
    } else {
        LOG_ERRORF("accept error: %d", errno);
        if (errno == EMFILE) {
            LOG_ERRORF("sockfd reached limit");
        }
    }
}
}  // namespace cai
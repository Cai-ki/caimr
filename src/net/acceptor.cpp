#include "net/acceptor.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>

#include "log/logging.h"
#include "net/address.h"

namespace cai {
static int create_nonblocking() {
    // 创建 TCP 套接字，设置非阻塞和执行时关闭标志
    // AF_INET ：
    // 地址族（Address Family），表示使用 IPv4 协议。
    // SOCK_STREAM ：
    // 套接字类型，表示面向连接的 TCP 协议（流式传输）。
    // SOCK_NONBLOCK ：
    // 非阻塞模式 ：
    // 默认情况下，套接字是阻塞的（如 accept()、read() 会阻塞进程直到完成）。
    // 设置此标志后，所有操作（如 connect()、recv()）立即返回，即使未完成。
    // SOCK_CLOEXEC ：
    // 执行时关闭 ：
    // 当进程调用 exec() 系列函数执行新程序时，自动关闭该套接字。
    // 避免新程序意外继承不需要的文件描述符。
    // IPPROTO_TCP ：
    // 指定传输层协议为 TCP（通常可以省略，因为 SOCK_STREAM 默认对应 TCP）。
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
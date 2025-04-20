#include "caimr/net/tcp_server.h"

#include <cstring>

#include "caimr/logger/logging.h"
#include "caimr/net/address.h"
#include "caimr/net/callbacks.h"
#include "caimr/net/eloop_tpool.h"

namespace cai {
namespace {
eloop* check_loop_not_null(eloop* loop) {
    if (!loop) {
        LOG_FATALF("main event loop is null");
    }
    return loop;
}
}  // namespace

tcp_server::tcp_server(eloop* loop, const address& listen_addr,
                       const std::string& name, option opt)
    : loop_(check_loop_not_null(loop)),
      ip_port_(listen_addr.to_ip_port()),
      name_(name),
      acceptor_(new acceptor(loop, listen_addr, opt == REUSE_PORT)),
      thread_pool_(new eloop_tpool(loop, name)),
      next_conn_id_(1),
      started_(0) {
    acceptor_->set_new_conn_callback(std::bind(&tcp_server::new_conn, this,
                                               std::placeholders::_1,
                                               std::placeholders::_2));
}

tcp_server::~tcp_server() {
    for (auto& item : conns_) {
        tcp_conn_ptr conn_ptr(item.second);
        item.second.reset();
        conn_ptr->get_loop()->run_in_loop(
            std::bind(&tcp_conn::destroy_conn, conn_ptr));
    }
}

void tcp_server::set_thread_num(int num_threads) {
    thread_pool_->set_thread_num(num_threads);
}

void tcp_server::start() {
    if (started_++ == 0) {
        thread_pool_->start(thread_init_callback_);
        loop_->run_in_loop(std::bind(&acceptor::listen, acceptor_.get()));
    }
}

void tcp_server::new_conn(int sockfd, const address& peer_addr) {
    eloop* io_loop = thread_pool_->get_next_loop();
    char buf[64];
    snprintf(buf, sizeof buf, "-%s#%d", ip_port_.c_str(), next_conn_id_++);
    std::string conn_name = name_ + buf;

    LOG_INFOF("new connection [%s] from %s", conn_name.c_str(),
              peer_addr.to_ip_port().c_str());

    sockaddr_in local;
    ::memset(&local, 0, sizeof local);
    socklen_t addrlen = sizeof local;
    if (::getsockname(sockfd, reinterpret_cast<sockaddr*>(&local), &addrlen) <
        0) {
        LOG_ERRORF("failed to get local address");
    }

    address local_addr(local);
    tcp_conn_ptr conn(
        new tcp_conn(io_loop, conn_name, sockfd, local_addr, peer_addr));

    conns_[conn_name] = conn;
    conn->set_conn_callback(conn_callback_);
    conn->set_msg_callback(msg_callback_);
    conn->set_write_complete_callback(write_complete_callback_);
    conn->set_close_callback(
        std::bind(&tcp_server::remove_conn, this, std::placeholders::_1));

    io_loop->run_in_loop(std::bind(&tcp_conn::establish_conn, conn));
}

void tcp_server::remove_conn(const tcp_conn_ptr& conn) {
    loop_->run_in_loop(std::bind(&tcp_server::remove_conn_in_loop, this, conn));
}

void tcp_server::remove_conn_in_loop(const tcp_conn_ptr& conn) {
    LOG_INFOF("removing connection %s", conn->name().c_str());
    conns_.erase(conn->name());
    eloop* io_loop = conn->get_loop();
    io_loop->queue_in_loop(std::bind(&tcp_conn::destroy_conn, conn));
}
}  // namespace cai
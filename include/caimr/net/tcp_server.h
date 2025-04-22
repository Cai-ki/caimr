#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "caimr/common/noncopyable.h"
#include "caimr/net/acceptor.h"
#include "caimr/net/address.h"
#include "caimr/net/buffer.h"
#include "caimr/net/callbacks.h"
#include "caimr/net/eloop.h"
#include "caimr/net/eloop_tpool.h"
#include "caimr/net/tcp_conn.h"

namespace cai {
class tcp_server : noncopyable {
   public:
    using thread_init_callback = std::function<void(eloop*)>;

    enum option {
        NO_REUSE_PORT,
        REUSE_PORT,
    };

    tcp_server(eloop* loop, const address& listen_addr, const std::string& name,
               option opt = NO_REUSE_PORT);
    ~tcp_server();

    void set_thread_init_callback(const thread_init_callback& cb) {
        thread_init_callback_ = cb;
    }
    void set_conn_callback(const close_callback& cb) { conn_callback_ = cb; }
    void set_msg_callback(const msg_callback& cb) { msg_callback_ = cb; }
    void set_write_complete_callback(const write_complete_callback& cb) {
        write_complete_callback_ = cb;
    }

    void set_thread_num(int num_threads);
    void start();

   private:
    void new_conn(int sockfd, const address& peer_addr);
    void remove_conn(const tcp_conn_ptr& conn);
    void remove_conn_in_loop(const tcp_conn_ptr& conn);

    using conn_map = std::unordered_map<uint32_t, tcp_conn_ptr>;

    eloop* loop_;  // main event loop
    const std::string ip_port_;
    const std::string name_;
    std::unique_ptr<acceptor> acceptor_;
    std::shared_ptr<eloop_tpool> thread_pool_;
    close_callback conn_callback_;
    msg_callback msg_callback_;
    write_complete_callback write_complete_callback_;
    thread_init_callback thread_init_callback_;
    std::atomic_int started_;
    uint32_t next_conn_id_;
    conn_map conns_;
};
}  // namespace cai
#pragma once

#include <functional>

#include "caimr/common/noncopyable.h"
#include "caimr/net/chan.h"
#include "socket.h"

namespace cai {
class eloop;
class address;

class acceptor : noncopyable {
   public:
    using new_conn_callback = std::function<void(int sockfd, const address&)>;

    acceptor(eloop* loop, const address& listen_addr, bool reuse_port);
    ~acceptor();

    void set_new_conn_callback(const new_conn_callback& cb) {
        new_conn_callback_ = cb;
    }

    bool listening() const { return listening_; }
    void listen();

   private:
    void handle_read();

    eloop* loop_;
    socket accept_socket_;
    chan accept_ch_;
    new_conn_callback new_conn_callback_;
    bool listening_;
};
}  // namespace cai
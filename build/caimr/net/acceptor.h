#pragma once

#include <functional>

#include "../common/noncopyable.h"
#include "./chan.h"
#include "socket.h"

namespace cai {
class eloop;
class address;

// acceptor 是对 socket 的再次封装，通过增加一层抽象来简化监听逻辑
class acceptor : noncopyable {
   public:
    using new_conn_callback = std::function<void(int sockfd, const address&)>;

    acceptor(eloop* loop, const address& listen_addr, bool reuse_port);
    ~acceptor();

    // 设置新连接建立后的回调
    void set_new_conn_callback(const new_conn_callback& cb) {
        new_conn_callback_ = cb;
    }
    // 返回监听状态
    bool listening() const { return listening_; }
    // 启动监听
    void listen();

   private:
    // 监听到新连接后的对应逻辑
    void handle_read();

    // 所属eloop
    eloop* loop_;
    // 对应的socket
    socket accept_socket_;
    // 对对应fd的封装，负责和eloop对接
    chan accept_ch_;
    // 新连接建立后的回调
    new_conn_callback new_conn_callback_;
    // 是否监听
    bool listening_;
};
}  // namespace cai
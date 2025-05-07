#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "../common/noncopyable.h"
#include "./acceptor.h"
#include "./address.h"
#include "./buffer.h"
#include "./callbacks.h"
#include "./eloop.h"
#include "./eloop_tpool.h"
#include "./tcp_conn.h"

namespace cai {
class tcp_server : noncopyable {
   public:
    using thread_init_callback = std::function<void(eloop*)>;

    enum option {  // 是否允许端口复用，枚举类型。
        NO_REUSE_PORT,
        REUSE_PORT,
    };

    tcp_server(eloop* loop, const address& listen_addr, const std::string& name,
               option opt = NO_REUSE_PORT);
    ~tcp_server();

    // 线程初始化回调
    void set_thread_init_callback(const thread_init_callback& cb) {
        thread_init_callback_ = cb;
    }
    void set_conn_callback(const close_callback& cb) { conn_callback_ = cb; }
    void set_msg_callback(const msg_callback& cb) { msg_callback_ = cb; }
    void set_write_complete_callback(const write_complete_callback& cb) {
        write_complete_callback_ = cb;
    }

    // 设置线程池大小
    void set_thread_num(int num_threads);
    void start();

   private:
    // 新建连接的回调
    void new_conn(int sockfd, const address& peer_addr);
    // 删除连接（安全的，可以多线程使用的）
    void remove_conn(const tcp_conn_ptr& conn);
    // 删除连接的具体实现（必须传入具体对象所属线程执行，也就是调用run_in_loop()。这样才能保证并发安全。）
    void remove_conn_in_loop(const tcp_conn_ptr& conn);

    using conn_map = std::unordered_map<uint32_t, tcp_conn_ptr>;

    eloop* loop_;  // main event loop
    const std::string ip_port_;
    const std::string name_;
    std::unique_ptr<acceptor> acceptor_;
    std::shared_ptr<eloop_tpool>
        thread_pool_;  // void new_conn(int sockfd, const address& peer_addr)
                       // 会被其它线程执行，要用std::shared_ptr保证存活。

    close_callback conn_callback_;
    msg_callback msg_callback_;
    write_complete_callback write_complete_callback_;
    thread_init_callback thread_init_callback_;

    // 启动次数
    std::atomic_int started_;
    // 下一个新连接id
    uint32_t next_conn_id_;
    // 维护已有连接
    conn_map conns_;
};
}  // namespace cai
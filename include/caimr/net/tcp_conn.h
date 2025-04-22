#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>

#include "caimr/common/noncopyable.h"
#include "caimr/common/timestamp.h"
#include "caimr/net/address.h"
#include "caimr/net/buffer.h"
#include "caimr/net/callbacks.h"

namespace cai {
class chan;
class eloop;
class socket;

class tcp_conn : noncopyable, public std::enable_shared_from_this<tcp_conn> {
   public:
    tcp_conn(eloop* loop, const uint32_t id, const std::string& name,
             int sockfd, const address& local_addr, const address& peer_addr);
    ~tcp_conn();

    eloop* get_loop() const { return loop_; }
    uint32_t id() const { return id_; }
    const std::string& name() const { return name_; }
    const address& local_address() const { return local_addr_; }
    const address& peer_address() const { return peer_addr_; }

    bool is_connected() const { return state_ == CONNECTED; }

    void send(const std::string& data);
    void shutdown();

    void set_conn_callback(const conn_callback& cb) { conn_callback_ = cb; }
    void set_msg_callback(const msg_callback& cb) { msg_callback_ = cb; }
    void set_write_complete_callback(const write_complete_callback& cb) {
        write_complete_callback_ = cb;
    }
    void set_close_callback(const close_callback& cb) { close_callback_ = cb; }
    void set_high_water_mark_callback(const high_water_mark_callback& cb,
                                      size_t mark) {
        high_water_mark_callback_ = cb;
        high_water_mark_ = mark;
    }

    void establish_conn();
    void destroy_conn();

   private:
    enum state { DISCONNECTED, CONNENTING, CONNECTED, DISCONNENTING };

    void set_state(state state) { state_.store(state); }
    void handle_read(timestamp receive_time);
    void handle_write();
    void handle_close();
    void handle_error();
    void send_in_loop(const void* data, size_t len);
    void shutdown_in_loop();

    eloop* loop_;
    const uint32_t id_;
    const std::string name_;
    std::atomic<state> state_;
    bool reading_;

    std::unique_ptr<socket> socket_;
    std::unique_ptr<chan> ch_;

    const address local_addr_;
    const address peer_addr_;

    conn_callback conn_callback_;
    msg_callback msg_callback_;
    write_complete_callback write_complete_callback_;
    high_water_mark_callback high_water_mark_callback_;
    close_callback close_callback_;
    size_t high_water_mark_;

    buffer input_buf_;
    buffer output_buf_;
};
}  // namespace cai
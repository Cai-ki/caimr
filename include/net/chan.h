#pragma once

#include <sys/epoll.h>

#include <functional>
#include <memory>

#include "../common/noncopyable.h"
#include "../common/time.h"

namespace cai {
class eloop;
// 对文件描述符的抽象，通过设置其感兴趣的事件，和对应事件下的回调能够方便poller和eloop处理事件。
// 对于poller来说只负责事件注册触发，eloop只负责触发后执行对应逻辑。
class chan : noncopyable {
   public:
    using callback = std::function<void()>;
    using rcallback = std::function<void(time::time_point)>;

    chan(eloop *loop, int fd);
    ~chan();

    // eloop::loop()中调用，执行触发事件后的逻辑
    void handle_event(time::time_point receive_time);

    // 设置回调
    void set_read_callback(rcallback cb) { read_callback_ = std::move(cb); }
    void set_write_callback(callback cb) { write_callback_ = std::move(cb); }
    void set_close_callback(callback cb) { close_callback_ = std::move(cb); }
    void set_error_callback(callback cb) { error_callback_ = std::move(cb); }

    // 绑定依赖的对象，执行的回调函数可能依赖于某些对象的存活，例如tcp_conn，因此逻辑上需要绑定，当执行chan的handle_event时会检查对象是否绑定，若绑定则会尝试将weak_ptr提升来判断其是否存活
    void tie(const std::shared_ptr<void> &);

    int fd() const { return fd_; }
    int events() const { return events_; }
    // 设置触发的事件类型
    void set_revents(int revt) { revents_ = revt; }

    // 设置fd感兴趣的事件状态 同步更新到epoll_ctl
    void enable_reading() {
        events_ |= READ_EVENT;
        update();
    }
    void disable_reading() {
        events_ &= ~READ_EVENT;
        update();
    }
    void enable_writing() {
        events_ |= WRITE_EVENT;
        update();
    }
    void disable_writing() {
        events_ &= ~WRITE_EVENT;
        update();
    }
    void disable_all() {
        events_ = NONE_EVENT;
        update();
    }

    // 返回当前的事件状态
    bool is_none_event() const { return events_ == NONE_EVENT; }
    bool is_writing() const { return events_ & WRITE_EVENT; }
    bool is_reading() const { return events_ & READ_EVENT; }

    int index() { return index_; }
    void set_index(int idx) { index_ = idx; }
    // 从属的eloop
    eloop *owner_loop() { return loop_; }
    // poller中注销
    void remove();

   private:
    void update();
    // 根据收到的event触发回调
    void handle_event_with_guard(time::time_point receive_time);

    static constexpr int NONE_EVENT = 0;
    static constexpr int READ_EVENT = EPOLLIN | EPOLLPRI;
    static constexpr int WRITE_EVENT = EPOLLOUT;

    eloop *loop_;
    const int fd_;
    // 感兴趣的事件
    int events_;
    // 触发的事件
    int revents_;
    // 存储者当前chan所拥有的fd在poller中的状态
    int index_;

    // 依赖对象的weak_ptr，可以判断其是否存活
    std::weak_ptr<void> tie_;
    bool tied_;

    // 各种触发事件的回调
    rcallback read_callback_;
    callback write_callback_;
    callback close_callback_;
    callback error_callback_;
};
}  // namespace cai
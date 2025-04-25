#pragma once

#include <sys/epoll.h>

#include <functional>
#include <memory>

#include "caimr/common/noncopyable.h"
#include "caimr/common/time.h"

namespace cai {
class eloop;

class chan : noncopyable {
   public:
    using callback = std::function<void()>;
    using rcallback = std::function<void(time::time_point)>;

    chan(eloop *loop, int fd);
    ~chan();

    void handle_event(time::time_point receive_time);

    void set_read_callback(rcallback cb) { read_callback_ = std::move(cb); }
    void set_write_callback(callback cb) { write_callback_ = std::move(cb); }
    void set_close_callback(callback cb) { close_callback_ = std::move(cb); }
    void set_error_callback(callback cb) { error_callback_ = std::move(cb); }

    void tie(const std::shared_ptr<void> &);

    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int revt) { revents_ = revt; }

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

    bool is_none_event() const { return events_ == NONE_EVENT; }
    bool is_writing() const { return events_ & WRITE_EVENT; }
    bool is_reading() const { return events_ & READ_EVENT; }

    int index() { return index_; }
    void set_index(int idx) { index_ = idx; }

    eloop *owner_loop() { return loop_; }
    void remove();

   private:
    void update();
    void handle_event_with_guard(time::time_point receive_time);

    static constexpr int NONE_EVENT = 0;
    static constexpr int READ_EVENT = EPOLLIN | EPOLLPRI;
    static constexpr int WRITE_EVENT = EPOLLOUT;

    eloop *loop_;
    const int fd_;
    int events_;
    int revents_;
    int index_;

    std::weak_ptr<void> tie_;
    bool tied_;

    rcallback read_callback_;
    callback write_callback_;
    callback close_callback_;
    callback error_callback_;
};
}  // namespace cai
#include "net/chan.h"

#include "log/logging.h"
#include "net/eloop.h"

namespace cai {
chan::chan(eloop *loop, int fd)
    : loop_(loop),
      fd_(fd),
      events_(0),
      revents_(0),
      index_(-1),
      tied_(false) {
}  // index_(-1)，-1代表目前fd状态是已建立，已添加是1，已删除是2

chan::~chan() {}

void chan::tie(const std::shared_ptr<void> &obj) {
    tie_ = obj;
    tied_ = true;
}

void chan::update() { loop_->update_chan(this); }

void chan::remove() { loop_->remove_chan(this); }

void chan::handle_event(time::time_point receive_time) {
    if (tied_) {
        std::shared_ptr<void> guard = tie_.lock();
        if (guard) {  // 如果提升失败，就不做任何处理
                      // 说明chan的回调依赖的对象已经死亡
            handle_event_with_guard(receive_time);
        }
    } else {
        handle_event_with_guard(receive_time);
    }
}

void chan::handle_event_with_guard(time::time_point receive_time) {
    LOG_INFOF("chan handle_event revents: %d\n", events_);

    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
        if (close_callback_) {
            close_callback_();
        }
    }

    if (revents_ & EPOLLERR) {
        if (error_callback_) {
            error_callback_();
        }
    }

    if (revents_ & (EPOLLIN | EPOLLPRI)) {
        if (read_callback_) {
            read_callback_(receive_time);
        }
    }

    if (revents_ & EPOLLOUT) {
        if (write_callback_) {
            write_callback_();
        }
    }
}
}  // namespace cai
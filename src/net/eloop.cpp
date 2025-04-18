#include "caimr/net/eloop.h"

#include <sys/eventfd.h>

#include <memory>

#include "caimr/logger/logging.h"
#include "caimr/net/chan.h"
#include "caimr/net/poller.h"
#include "caimr/thread/cthread.h"

namespace cai {
__thread eloop* t_loop_in_this_thread = nullptr;

constexpr int POLL_TIME_MS = 10000;

int create_eventfd() {
    int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0) {
        LOG_FATALF("eventfd error:%d\n", errno);
    }
    return evtfd;
}

eloop::eloop()
    : looping_(false),
      quit_(false),
      calling_pending_functors_(false),
      thread_id_(cthread::tid()),
      poller_(poller::new_default_poller(this)),
      wakeup_fd_(create_eventfd()),
      wakeup_ch_(new chan(this, wakeup_fd_)),
      current_active_ch_(nullptr) {
    LOG_DEBUGF("eloop created %p in thread %d \n", this, thread_id_);
    if (t_loop_in_this_thread) {
        LOG_FATALF("another eloop %p exists in this thread %d \n",
                   t_loop_in_this_thread, thread_id_);
    } else {
        t_loop_in_this_thread = this;
    }

    wakeup_ch_->set_read_callback(std::bind(&eloop::handle_read, this));
    wakeup_ch_->enable_reading();
}

eloop::~eloop() {
    wakeup_ch_->disable_all();
    wakeup_ch_->remove();
    close(wakeup_fd_);
    t_loop_in_this_thread = nullptr;
}

void eloop::handle_read() {
    uint64_t one = 1;
    ssize_t n = read(wakeup_fd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG_ERRORF("eloop::handle_read() reads %zd bytes instead of 8", n);
    }
}

void eloop::loop() {
    looping_.store(true);
    quit_.store(false);
    LOG_INFOF("eloop %p start looping \n", this);

    while (!quit_.load()) {
        active_chs_.clear();
        poll_return_time_ = poller_->poll(POLL_TIME_MS, &active_chs_);

        for (chan* ch : active_chs_) {
            current_active_ch_ = ch;
            current_active_ch_->handle_event(poll_return_time_);
        }
        current_active_ch_ = nullptr;
        do_pending_functors();
    }

    LOG_INFOF("eloop %p stop looping. \n", this);
    looping_.store(false);
}

void eloop::quit() {
    quit_.store(true);
    if (!is_in_loop_thread()) {
        wakeup();
    }
}

void eloop::run_in_loop(functor cb) {
    if (is_in_loop_thread()) {
        cb();
    } else {
        queue_in_loop(std::move(cb));
    }
}

void eloop::queue_in_loop(functor cb) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pending_functors_.emplace_back(std::move(cb));
    }

    if (!is_in_loop_thread() || calling_pending_functors_) {
        wakeup();
    }
}

void eloop::wakeup() {
    uint64_t one = 1;
    ssize_t n = write(wakeup_fd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG_ERRORF("eloop::wakeup() writes %zd bytes instead of 8", n);
    }
}

void eloop::update_chan(chan* ch) { poller_->update_chan(ch); }

void eloop::remove_chan(chan* ch) { poller_->remove_chan(ch); }

bool eloop::has_chan(chan* ch) { return poller_->has_chan(ch); }

void eloop::do_pending_functors() {
    std::vector<functor> functors;
    calling_pending_functors_.store(true);

    {
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pending_functors_);
    }

    for (const functor& functor : functors) {
        functor();
    }

    calling_pending_functors_.store(false);
}
}  // namespace cai
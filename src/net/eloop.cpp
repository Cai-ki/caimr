#include "net/eloop.h"

#include <sys/eventfd.h>

#include <memory>

#include "logger/logging.h"
#include "net/chan.h"
#include "net/poller.h"
#include "thread/cthread.h"

namespace cai {
// TLS 确保 one thread per loop
thread_local eloop* t_loop_in_this_thread = nullptr;
// poll 最大阻塞时间
constexpr int POLL_TIME_MS = 10000;

int create_eventfd() {
    int evtfd = eventfd(
        0, EFD_NONBLOCK | EFD_CLOEXEC);  // eventfd 是 Linux 内核提供的一个
                                         // 高效的线程间/进程间同步机制
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
      wakeup_ch_(new chan(this, wakeup_fd_)) {
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
            ch->handle_event(poll_return_time_);
        }
        do_pending_functors();
    }

    LOG_INFOF("eloop %p stop looping. \n", this);
    looping_.store(false);
}

void eloop::quit() {
    quit_.store(true);
    if (!is_in_loop_thread()) {  // 若不在本线程则需要唤醒
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
    {  // 加锁并放入任务队列
        std::unique_lock<std::mutex> lock(mutex_);
        pending_functors_.emplace_back(std::move(cb));
    }

    if (!is_in_loop_thread() ||
        calling_pending_functors_
            .load()) {  // 不在 eloop
                        // 所在线程或者当前正在执行任务函数都需要唤醒对应线程，防止新任务处理延迟
        wakeup();
    }
}

void eloop::wakeup() {
    uint64_t one = 1;
    ssize_t n = write(wakeup_fd_, &one, sizeof(one));  // 写入数据触发事件
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

    {  // lock and swap 操作能提高性能，相比于新建一个 std::vector
       // 的开销，由于任务执行时间较长，加锁执行所有任务会影响新任务放入任务队列
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pending_functors_);
    }

    for (const functor& functor : functors) {
        functor();
    }

    calling_pending_functors_.store(false);
}
}  // namespace cai
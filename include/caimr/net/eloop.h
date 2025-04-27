#pragma once
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include "caimr/common/noncopyable.h"
#include "caimr/common/time.h"
#include "caimr/thread/cthread.h"

namespace cai {
class chan;
class poller;

class eloop : public noncopyable {
   public:
    using functor = std::function<void()>;

    eloop();
    ~eloop();

    // 循环
    void loop();
    // 退出循环
    void quit();
    // poll 返回时间的时间点
    time::time_point poll_return_time() const { return poll_return_time_; }

    // 在 eloop 所在线程执行函数
    void run_in_loop(functor cb);
    // 将任务函数放进 eloop
    // 的任务队列中（主要用于不同线程任务传递，确保任务被指定 eloop
    // 所在的线程执行）
    void queue_in_loop(functor cb);
    // 唤醒 eloop ，由于 poll 的阻塞机制，需要专门触发事件来唤醒循环体执行
    void wakeup();

    // 调用拥有的 poller 对象的方法
    void update_chan(chan* ch);
    void remove_chan(chan* ch);
    bool has_chan(chan* ch);

    // 判断当前是否属于 eloop 所属线程
    bool is_in_loop_thread() const { return thread_id_ == cthread::tid(); }

   private:
    // 配合 wakeup 使用，作为 wakeup_ch_ 的回调函数，读取用来唤醒循环的数据
    void handle_read();
    // 执行交给 eloop 所在线程的所有任务函数
    void do_pending_functors();

    using chan_list = std::vector<chan*>;

    // 原子变量表示状态
    std::atomic<bool> looping_;
    std::atomic<bool> quit_;
    std::atomic<bool> calling_pending_functors_;
    const pid_t thread_id_;
    time::time_point poll_return_time_;

    // poller 对象，用于管理 chan ，而 chan 就是对文件描述符的封装，当然 poller
    // 也就是对 epoll 等的封装
    std::unique_ptr<poller> poller_;

    // 用于唤醒循环的文件描述符，以及对应封装好的 chan
    int wakeup_fd_;
    std::unique_ptr<chan> wakeup_ch_;

    chan_list active_chs_;

    std::vector<functor> pending_functors_;
    std::mutex mutex_;
};
}  // namespace cai
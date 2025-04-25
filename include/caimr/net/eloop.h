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

    void loop();
    void quit();
    time::time_point poll_return_time() const { return poll_return_time_; }

    void run_in_loop(functor cb);
    void queue_in_loop(functor cb);
    void wakeup();

    void update_chan(chan* ch);
    void remove_chan(chan* ch);
    bool has_chan(chan* ch);

    bool is_in_loop_thread() const { return thread_id_ == cthread::tid(); }

   private:
    void handle_read();
    void do_pending_functors();

    using chanlist = std::vector<chan*>;

    std::atomic<bool> looping_;
    std::atomic<bool> quit_;
    std::atomic<bool> calling_pending_functors_;
    const pid_t thread_id_;
    time::time_point poll_return_time_;

    std::unique_ptr<poller> poller_;

    int wakeup_fd_;
    std::unique_ptr<chan> wakeup_ch_;

    chanlist active_chs_;
    chan* current_active_ch_;

    std::vector<functor> pending_functors_;
    std::mutex mutex_;
};
}  // namespace cai
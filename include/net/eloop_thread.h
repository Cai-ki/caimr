#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>

#include "../common/noncopyable.h"
#include "../thread/thread.h"

namespace cai {
class eloop;

class eloop_thread : noncopyable {
   public:
    using thread_init_callback = std::function<void(eloop*)>;

    eloop_thread(
        const thread_init_callback& init_callback = thread_init_callback(),
        const std::string& name = std::string());
    ~eloop_thread();

    eloop* start_loop();

   private:
    void thread_func();

    eloop* loop_;
    bool exiting_;
    thread thread_;
    std::mutex mutex_;
    std::condition_variable condition_;
    thread_init_callback init_callback_;
};
}  // namespace cai
#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>
#include <vector>

#include "caimr/common/noncopyable.h"
#include "thread.h"

namespace cai {
class tpool : noncopyable {
   public:
    using tfunc = std::function<void()>;

    explicit tpool(const std::string& name = "tpool");
    ~tpool();

    void set_tcallback(const tfunc& cb) { t_init_callback_ = cb; }
    void set_tsize(const int& num) { tsize_ = num; }
    void start();
    void stop();

    const std::string& name() const { return name_; }
    size_t qsize() const;

    void add(tfunc tf);

   private:
    bool is_full() const;
    void run();

    mutable std::mutex mutex_;
    std::condition_variable cond_;
    std::string name_;
    tfunc t_init_callback_;
    std::vector<std::unique_ptr<thread>> threads_;
    std::deque<tfunc> queue_;
    bool running_;
    size_t tsize_;
};
}  // namespace cai
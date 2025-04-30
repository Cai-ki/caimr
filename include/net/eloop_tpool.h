#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "../common/noncopyable.h"

namespace cai {
class eloop;
class eloop_thread;

class eloop_tpool : noncopyable {
   public:
    using thread_tnit_callback = std::function<void(eloop*)>;

    eloop_tpool(eloop* base_loop, const std::string& name);
    ~eloop_tpool();

    void set_thread_num(int num_threads) { num_threads_ = num_threads; }
    void start(const thread_tnit_callback& cb = thread_tnit_callback());
    eloop* get_next_loop();
    std::vector<eloop*> get_all_loops();
    bool is_started() const { return started_; }
    const std::string& name() const { return name_; }

   private:
    eloop* base_loop_;
    std::string name_;
    bool started_;
    int num_threads_;
    int next_idx_;
    std::vector<std::unique_ptr<eloop_thread>> threads_;
    std::vector<eloop*> loops_;
};
}  // namespace cai
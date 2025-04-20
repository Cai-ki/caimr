#include "caimr/net/eloop_tpool.h"

#include "caimr/net/eloop_thread.h"

namespace cai {
eloop_tpool::eloop_tpool(eloop* base_loop, const std::string& name)
    : base_loop_(base_loop),
      name_(name),
      started_(false),
      num_threads_(0),
      next_idx_(0) {}

eloop_tpool::~eloop_tpool() = default;

void eloop_tpool::start(const thread_tnit_callback& cb) {
    started_ = true;

    for (int i = 0; i < num_threads_; ++i) {
        char buf[name_.size() + 32];
        snprintf(buf, sizeof buf, "%s%d", name_.c_str(), i);
        threads_.emplace_back(
            std::make_unique<eloop_thread>(cb, std::string(buf)));
        loops_.push_back(threads_.back()->start_loop());
    }

    if (num_threads_ == 0 && cb) {
        cb(base_loop_);
    }
}

eloop* eloop_tpool::get_next_loop() {
    eloop* loop = base_loop_;

    if (!loops_.empty()) {
        loop = loops_[next_idx_];
        next_idx_ = (next_idx_ + 1) % loops_.size();
    }
    return loop;
}

std::vector<eloop*> eloop_tpool::get_all_loops() {
    return loops_.empty() ? std::vector<eloop*>{base_loop_} : loops_;
}
}  // namespace cai
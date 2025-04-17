#include "caimr/thread/tpool.h"

namespace cai {
tpool::tpool(const std::string& name)
    : mutex_(), cond_(), name_(name), running_(false), tsize_(0) {}

tpool::~tpool() {
    stop();
    for (const auto& t : threads_) {
        t->join();
    }
}

void tpool::start() {
    running_ = true;
    threads_.reserve(tsize_);
    for (int i = 0; i < tsize_; ++i) {
        char id[32];
        snprintf(id, sizeof(id), "%d", i + 1);
        threads_.emplace_back(
            new thread(std::bind(&tpool::run, this), name_ + id));
        threads_[i]->start();
    }
    if (tsize_ == 0 && t_init_callback_) {
        t_init_callback_();
    }
}

void tpool::stop() {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = false;
    cond_.notify_all();
}

size_t tpool::qsize() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

void tpool::add(tfunc tf) {
    std::unique_lock<std::mutex> lock(mutex_);
    queue_.push_back(tf);
    cond_.notify_one();
}

void tpool::run() {
    try {
        if (t_init_callback_) {
            t_init_callback_();
        }
        tfunc task;
        while (true) {
            {
                std::unique_lock<std::mutex> lock(mutex_);
                while (queue_.empty()) {
                    if (!running_) {
                        return;
                    }
                    cond_.wait(lock);
                }
                task = queue_.front();
                queue_.pop_front();
            }
            if (task != nullptr) {
                task();
            }
        }
    } catch (...) {
        // LOG_WARN << "run throw exception";
    }
}
}  // namespace cai
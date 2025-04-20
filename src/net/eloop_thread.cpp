#include "caimr/net/eloop_thread.h"

#include "caimr/net/eloop.h"

namespace cai {
eloop_thread::eloop_thread(const thread_init_callback& init_callback,
                           const std::string& name)
    : loop_(nullptr),
      exiting_(false),
      thread_(std::bind(&eloop_thread::thread_func, this), name),
      init_callback_(init_callback) {}

eloop_thread::~eloop_thread() {
    exiting_ = true;
    if (loop_) {
        loop_->quit();
        thread_.join();
    }
}

eloop* eloop_thread::start_loop() {
    thread_.start();

    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock, [this] { return loop_ != nullptr; });
    return loop_;
}

void eloop_thread::thread_func() {
    eloop loop;

    if (init_callback_) {
        init_callback_(&loop);
    }

    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        condition_.notify_one();
    }

    loop.loop();

    std::unique_lock<std::mutex> lock(mutex_);
    loop_ = nullptr;
}
}  // namespace cai
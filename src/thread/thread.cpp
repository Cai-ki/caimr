#include "caimr/thread/thread.h"

#include <semaphore.h>

#include <future>
#include <memory>

namespace cai {
std::atomic_int thread::tcount_(0);

thread::thread(tfunc func, const std::string& name)
    : started_(false),
      joined_(false),
      tid_(0),
      func_(std::move(func)),
      name_(name) {
    set_default();
}

thread::~thread() {
    if (started_ && !joined_)
        thread_->detach();  // 分离线程，后台运行，分离后无法再等待
}

void thread::start() {
    started_ = true;
    std::promise<void> promise;
    std::future<void> future = promise.get_future();
    // std::promise 和 std::future 属于 C++ 标准库 ，定义在头文件 <future>
    // 中。它们是 C++11 引入的线程同步工具，用于在不同线程之间传递数据或信号 。
    thread_ = std::make_unique<std::thread>([&]() {
        tid_ = thread::tid();
        promise.set_value();
        func_();
    });
    future.wait();
}

void thread::join() {
    joined_ = true;
    thread_->join();  // 阻塞等待线程结束，调用后线程不可再操作
}

void thread::set_default() {
    if (name_.empty()) {
        char buf[32];
        snprintf(buf, sizeof(buf), "thread%d", ++tcount_);
        name_ = buf;
    }
}
}  // namespace cai
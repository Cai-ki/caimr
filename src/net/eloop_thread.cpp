#include "net/eloop_thread.h"

#include "net/eloop.h"

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
        loop_->quit();   // 结束loop
        thread_.join();  // 释放thread
    }
}

eloop* eloop_thread::start_loop() {
    thread_.start();  // 启动线程，执行eloop_thread::thread_func()，

    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(
        lock, [this] { return loop_ != nullptr; });  // 阻塞直到loop_初始化完成
    return loop_;
}

void eloop_thread::thread_func() {
    eloop loop;

    if (init_callback_) {  // 执行初始化回调
        init_callback_(&loop);
    }

    {  // 加锁，初始化loop_，通知对应的condition_.wait线程
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        condition_.notify_one();
    }

    // 在这之后就不需要了，eloop已经初始化完成，之后是否正在执行loop都无所谓，任务本身是可以先缓存的
    loop.loop();

    // loop的生命周期结束，绑定于具体线程，当eloop_thread::thread_func()执行完毕就可以设为空，loop本身就是个栈变量自然释放
    std::unique_lock<std::mutex> lock(mutex_);
    loop_ = nullptr;
}
}  // namespace cai
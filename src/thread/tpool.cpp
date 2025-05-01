#include "thread/tpool.h"

#include "log/logging.h"
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
    threads_.reserve(tsize_);  // 预分配指定数量的存储空间
    for (int i = 0; i < tsize_; ++i) {
        char id[32];
        snprintf(id, sizeof(id), "%d", i + 1);
        threads_.emplace_back(
            new thread(std::bind(&tpool::run, this), name_ + id));
        threads_[i]->start();
    }
    if (tsize_ == 0 && t_init_callback_) {  // 线程池为空时触发一次回调
        t_init_callback_();
    }
}

void tpool::stop() {  // 加锁后设置状态，并通知所有线程
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = false;
    cond_
        .notify_all();  // 唤醒所有线程，竞争执行剩余任务，任务队列为空后所有线程都会观测到
                        // running_
                        // 的修改后的值，并退出运行。如若不通知或者仅通知部分，可能有线程一直等待唤醒，导致线程泄漏
}

size_t tpool::qsize() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

void tpool::add(tfunc tf) {
    std::unique_lock<std::mutex> lock(mutex_);  // 与条件变量配合使用
    queue_.push_back(tf);
    cond_.notify_one();  // 单任务唤醒一个线程即可
}

void tpool::run() {
    try {
        if (t_init_callback_) {
            t_init_callback_();
        }
        tfunc task;
        while (true) {
            {  // code block 结束后锁自动释放
                std::unique_lock<std::mutex> lock(mutex_);
                while (queue_.empty()) {
                    if (!running_) {
                        return;
                    }
                    cond_.wait(lock);  // 主动释放锁并等待唤醒
                }
                // 若任务队列非空则正常执行任务
                task = queue_.front();
                queue_.pop_front();
            }
            if (task != nullptr) {
                task();
            }
        }
    } catch (...) {  // 防止任务抛出异常影响全局
        LOG_WARN << "run throw exception";
    }
}
}  // namespace cai
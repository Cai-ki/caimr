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
    // 任务函数类型
    using tfunc = std::function<void()>;

    explicit tpool(const std::string& name = "tpool");
    ~tpool();

    // 设置线程初始化后的回调
    void set_tcallback(const tfunc& cb) { t_init_callback_ = cb; }
    // 设置线程池大小
    void set_tsize(const int& num) { tsize_ = num; }
    // 启动线程池
    void start();
    // 停止线程池
    void stop();
    // 线程池名称
    const std::string& name() const { return name_; }
    // 队列中任务数量
    size_t qsize() const;
    // 添加任务到任务队列
    void add(tfunc tf);

   private:
    // 线程绑定的 run 函数，负责执行 add 进来的 tfunc
    void run();

    // mutable 允许 const 修饰的函数修改
    mutable std::mutex mutex_;
    // 条件变量，与 std::unique_lock<std::mutex> 配合便于协调线程的调度
    std::condition_variable cond_;
    std::string name_;
    // 线程构建前的回调
    tfunc t_init_callback_;
    // 存储线程实例
    std::vector<std::unique_ptr<thread>> threads_;
    // 使用双端队列作为任务队列，为什么不用 std::vector ？这是因为后者不符合
    // FIFO ，同时 std::deque
    // 内存是分段分配的（由多个固定大小的块组成），不需要连续的内存空间。扩容时不会导致已有元素的内存地址失效（迭代器失效问题较少）。
    // 对应任务队列需要频繁的插入删除来说更适合。而 std::queue 底层一般用
    // std::deque 实现，所以使用 std::deque 即可
    std::deque<tfunc> queue_;
    bool running_;
    size_t tsize_;
};
}  // namespace cai
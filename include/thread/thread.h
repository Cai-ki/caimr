#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <thread>

#include "../common/noncopyable.h"

namespace cai {
class thread : noncopyable {
   public:
    // 线程运行的函数
    using tfunc = std::function<void()>;
    explicit thread(tfunc func, const std::string& name = {});
    ~thread();

    // 启动线程
    void start();
    // 阻塞等待线程结束
    void join();
    // 是否启动
    bool started() const { return started_; }
    // 获得当前线程 id
    pid_t tid() const { return tid_; }
    const std::string& name() const { return name_; }

   private:
    // 默认 name 设置
    void set_default();
    bool started_;
    bool joined_;
    std::unique_ptr<std::thread> thread_;
    pid_t tid_;
    tfunc func_;
    std::string name_;
    // 类静态原子变量，用于记录线程创建数量
    static std::atomic_int tcount_;
};
}  // namespace cai
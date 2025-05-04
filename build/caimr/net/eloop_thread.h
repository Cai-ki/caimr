#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>

#include "../common/noncopyable.h"
#include "../thread/thread.h"

namespace cai {
class eloop;

// 对eloop_thread的封装，相较于两个分开初始化，这个类将两个流程整合起来，简化之后的操作，为eloop_thread做准备
// 当然封装后就会限制一些流程，但创建thread不用绑定eloop会节省不少重复劳作
class eloop_thread : noncopyable {
   public:
    using thread_init_callback = std::function<void(eloop*)>;

    eloop_thread(
        const thread_init_callback& init_callback = thread_init_callback(),
        const std::string& name = std::string());
    ~eloop_thread();

    // 启动loop返回对应eloop，start_loop->thread_func->start_loop，条件变量确保thread_func的初始化流程在start_loop执行中执行完毕
    eloop* start_loop();

   private:
    // 传入thread的func
    void thread_func();

    eloop* loop_;
    bool exiting_;
    thread thread_;
    std::mutex mutex_;
    // 条件变量，确保初始化的流程正常进行
    std::condition_variable condition_;
    // 线程初始化的回调
    thread_init_callback init_callback_;
};
}  // namespace cai
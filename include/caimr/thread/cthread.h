#pragma once

#include <sys/syscall.h>
#include <unistd.h>

namespace cai {
namespace cthread {
extern thread_local int tid_;
//  线程局部存储（TLS），__thread是 GCC、Clang
// 等编译器提供的非标准扩展，thread_local是 C++11
// 标准引入的关键字，提供统一的跨平台线程局部存储支持。
inline void cache() { tid_ = static_cast<pid_t>(::syscall(SYS_gettid)); }
inline int tid() {
    if (__builtin_expect(tid_ == 0, 0)) cache();
    // long __builtin_expect(long exp, long c)，__builtin_expect 是 GCC
    // 编译器提供的一个内建函数 ，用于分支预测优化 。
    // __builtin_expect(tid_ == 0, 0) 表示：期望 tid_ == 0 的结果为
    // 0（即条件不成立） 。编译器会将 if 分支（即 cache()
    // 的调用）优化为不太可能执行的路径 ，从而提升常规路径（tid_
    // 已初始化）的性能。
    return tid_;
}
}  // namespace cthread
}  // namespace cai
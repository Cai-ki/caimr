#pragma once

#include <sys/syscall.h>
#include <unistd.h>

namespace cai {
namespace cthread {
extern thread_local int tid_;  // 线程局部存储
void cache();
inline int tid() {
    if (__builtin_expect(tid_ == 0, 0)) cache();
    return tid_;
}
}  // namespace cthread
}  // namespace cai
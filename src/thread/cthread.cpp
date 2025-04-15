#include "caimr/thread/cthread.h"

namespace cthread {
__thread int tid_ = 0;

void cache() {
    if (tid_ == 0) {
        tid_ = static_cast<pid_t>(::syscall(SYS_gettid));
    }
}
}  // namespace cthread
#include "caimr/thread/cthread.h"

namespace cai {
namespace cthread {
thread_local int tid_ = 0;
// extern thread_local int tid_
// 如果在头文件中直接定义变量（非extern），多个源文件包含该头文件会导致多重定义错误
// 。 C/C++ 要求全局变量（非inline 或 static）在所有编译单元中只能有一个定义
// （One Definition Rule，ODR）。
}  // namespace cthread
}  // namespace cai
#pragma once

#include <chrono>
#include <string>
namespace cai {
class time {
   public:
    using clock = std::chrono::system_clock;
    using time_point = std::chrono::time_point<clock>;
    using duration = std::chrono::microseconds;
    // 返回只读的 C 风格字符串（线程安全）
    static const char* to_cstring(const time_point& tp) {
        std::time_t t = clock::to_time_t(tp);
        std::tm tm_buf;

// 跨平台兼容性处理
#if defined(_WIN32) || defined(_WIN64)
        localtime_s(&tm_buf, &t);
#else
        localtime_r(&t, &tm_buf);
#endif

        // 线程局部缓冲区（避免竞争）
        thread_local static char buffer[64];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_buf);

        return buffer;
    }

    // 返回 std::string
    static std::string to_string(const time_point& tp) {
        return std::string(to_cstring(tp));
    }
};
}  // namespace cai
#pragma once

namespace cai {
class noncopyable {
   public:
    noncopyable(const noncopyable &) = delete;
    noncopyable &operator=(const noncopyable &) = delete;

   protected:
    noncopyable() = default;   // 允许派生类构造
    ~noncopyable() = default;  // 允许派生类析构
};
}  // namespace cai
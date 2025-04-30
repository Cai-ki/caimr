#pragma once

namespace cai {
// 通过继承
// noncopyable，派生类会自动禁用拷贝构造函数和拷贝赋值运算符，从而防止对象被意外拷贝或赋值。
class noncopyable {
   public:
    noncopyable(const noncopyable &) = delete;  // 删除拷贝构造。
    noncopyable &operator=(const noncopyable &) = delete;  // 删除赋值运算符。

   protected:
    // 为什么构造函数和析构函数是 protected？
    // 如果构造函数是
    // private，派生类无法调用基类构造函数，导致编译失败。如果析构函数是
    // private，派生类无法析构基类部分，导致编译失败。 通过设为
    // protected，派生类可以正常构造和析构，但外部代码无法直接实例化 noncopyable
    // 对象。
    // 使用 = default 显式要求编译器生成默认实现，避免编译器自动生成其他版本。
    noncopyable() = default;   // 允许派生类构造。
    ~noncopyable() = default;  // 允许派生类析构。
};
}  // namespace cai
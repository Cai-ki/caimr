#include "caimr/common/noncopyable.h"

#include <iostream>

class TestClass : cai::noncopyable {
   public:
    TestClass() = default;
};

int main() {
    TestClass obj1;
    // 以下两行代码应该导致编译错误（手动取消注释测试）
    // TestClass obj2 = obj1;  // 测试拷贝构造
    // TestClass obj3;
    // obj3 = obj1;  // 测试赋值运算符

    // 用static_assert验证类型特性
    static_assert(!std::is_copy_constructible<TestClass>::value,
                  "TestClass should not be copy constructible");
    static_assert(!std::is_copy_assignable<TestClass>::value,
                  "TestClass should not be copy assignable");

    std::cout << "noncopyable ok!" << std::endl;
    return 0;
}
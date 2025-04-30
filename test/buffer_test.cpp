#include "net/buffer.h"

#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <cstring>
#include <iostream>

void test_basic_operations() {
    cai::buffer buf;
    assert(buf.readable_bytes() == 0);

    // 测试基本写入
    const char* data = "hello";
    buf.append(data, 5);
    assert(buf.readable_bytes() == 5);

    // 测试读取
    std::string s = buf.retrieve_as_string(3);
    assert(s == "hel");
    assert(buf.readable_bytes() == 2);

    // 测试空间扩展
    std::vector<char> big_data(2048, 'a');
    buf.append(big_data.data(), big_data.size());
    assert(buf.writable_bytes() >= 0);  // 验证没有崩溃
}

void test_read_write_fd() {
    int fds[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, fds);

    // 测试写fd
    cai::buffer send_buf;
    send_buf.append("test message", 12);
    int err = 0;
    ssize_t n = send_buf.write_fd(fds[0], &err);
    assert(n == 12);

    // 测试读fd
    cai::buffer recv_buf;
    n = recv_buf.read_fd(fds[1], &err);
    assert(n == 12);
    assert(recv_buf.retrieve_all_as_string() == "test message");

    close(fds[0]);
    close(fds[1]);
}

int main() {
    test_basic_operations();
    test_read_write_fd();
    std::cout << "all tests passed!" << std::endl;
    return 0;
}
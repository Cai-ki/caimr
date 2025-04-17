#include "caimr/net/socket.h"

#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <cstring>
#include <iostream>
#include <thread>

#include "caimr/net/address.h"

// 辅助函数：创建简易客户端连接
void client_connect(uint16_t port) {
    int cli = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    connect(cli, (sockaddr*)&serv_addr, sizeof(serv_addr));
    close(cli);
}

// 测试基础功能
void test_basic() {
    int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    cai::socket s(sockfd);
    assert(s.fd() > 0);  // 检查文件描述符有效性
    std::cout << "test_basic passed.\n";
}

void test_bind_listen() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    cai::socket s(sockfd);

    address addr(0);  // 初始端口为0
    s.bind_address(addr);
    s.listen();

    // 获取实际绑定地址
    sockaddr_in local_addr{};
    socklen_t len = sizeof(local_addr);
    getsockname(sockfd, (sockaddr*)&local_addr, &len);
    uint16_t port = ntohs(local_addr.sin_port);

    // 关键修改：用实际端口构造新地址
    address real_addr(local_addr);  // 使用新构造函数

    // 尝试绑定到真实端口应失败
    int testfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(::bind(testfd, (sockaddr*)real_addr.sock_addr(),
                  sizeof(sockaddr_in)) == -1);
    close(testfd);

    std::cout << "test_bind_listen passed. used port: " << port << "\n";
}

// 测试选项设置
void test_socket_options() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    cai::socket s(sockfd);

    s.set_tcp_no_delay(true);
    s.set_reuse_addr(true);

    // 验证选项设置
    int optval;
    socklen_t len = sizeof(optval);
    getsockopt(s.fd(), IPPROTO_TCP, TCP_NODELAY, &optval, &len);
    assert(optval == 1);

    getsockopt(s.fd(), SOL_SOCKET, SO_REUSEADDR, &optval, &len);
    assert(optval == 1);

    std::cout << "test_socket_options passed.\n";
}

// 测试接受连接
void test_accept() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    cai::socket s(sockfd);

    address addr(0);  // 动态端口
    s.bind_address(addr);
    s.listen();

    // 获取实际端口号
    sockaddr_in local_addr{};
    socklen_t len = sizeof(local_addr);
    getsockname(sockfd, (sockaddr*)&local_addr, &len);
    uint16_t port = ntohs(local_addr.sin_port);

    // 启动客户端线程
    std::thread client(client_connect, port);

    // 接受连接
    address peer;
    int connfd = s.accept(&peer);
    assert(connfd > 0);

    client.join();
    close(connfd);
    std::cout << "test_accept passed.\n";
}

int main() {
    test_basic();
    test_bind_listen();
    test_socket_options();
    test_accept();
    std::cout << "all tests passed!\n";
    return 0;
}
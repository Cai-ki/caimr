#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>

#include <string>

namespace cai {
class address {
   public:
    // explicit 关键字用于禁止编译器进行隐式类型转换
    // ，通常用于修饰单参数构造函数 或用户定义的转换函数 。
    explicit address(uint16_t port = 0, std::string ip = "127.0.0.1");
    // struct sockaddr_in {
    //     sa_family_t sin_family;   // 地址族（AF_INET 表示 IPv4）
    //     uint16_t sin_port;        // 端口号（网络字节序）
    //     struct in_addr sin_addr;  // IPv4 地址（网络字节序）
    // };
    explicit address(const sockaddr_in &addr) : addr_(addr) {}

    std::string to_ip() const;
    std::string to_ip_port() const;
    uint16_t to_port() const;

    const sockaddr_in *sock_addr() const { return &addr_; }
    void set_sock_addr(const sockaddr_in &addr) { addr_ = addr; }

   private:
    sockaddr_in addr_;
};
}  // namespace cai
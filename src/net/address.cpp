#include "caimr/net/address.h"

#include <string.h>
#include <strings.h>

namespace cai {
address::address(uint16_t port, std::string ip) {
    ::memset(&addr_, 0, sizeof(addr_));  // 初始化 addr_ 结构体为 0
    addr_.sin_family = AF_INET;
    addr_.sin_port =
        ::htons(port);  // uint16_t htons(uint16_t hostshort); 主机字节序 ->
                        // 网络字节序。处理端口号的字节序转换。
    ::inet_pton(
        AF_INET, ip.c_str(),
        &addr_.sin_addr);  // int inet_pton(int af, const char *src, void *dst);
                           // 将字符串 IP 地址（如
                           // "127.0.0.1"）转换为二进制形式（网络字节序）。
}

std::string address::to_ip() const {
    char buf[64] = {0};
    ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof buf);
    return buf;
}

std::string address::to_ip_port() const {
    char buf[64] = {0};
    ::inet_ntop(AF_INET, &addr_.sin_addr, buf,
                sizeof buf);  // const char *inet_ntop(int af, const void *src,
                              // char *dst, socklen_t size); 将二进制 IP
                              // 地址转换为字符串形式。
    size_t end = ::strlen(buf);
    uint16_t port = ::ntohs(
        addr_.sin_port);  // uint16_t ntohs(uint16_t netshort);
                          // 网络字节序 -> 主机字节序。处理端口号的字节序转换。
    snprintf(buf + end, sizeof(buf) - end, ":%u",
             port);  // 防止缓冲区溢出，此处无意义仅作防御性编程
    return buf;
}

uint16_t address::to_port() const { return ::ntohs(addr_.sin_port); }
}  // namespace cai
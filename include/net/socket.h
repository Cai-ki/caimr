#pragma once

#include "../common/noncopyable.h"
namespace cai {
class address;
// Socket（套接字） 是计算机网络中实现 进程间通信（IPC）
// 的核心机制，它提供了一种在不同主机（或同一主机）上的应用程序之间交换数据的标准方法。
// Socket 是一个 抽象接口 ，封装了底层网络协议（如
// TCP/IP）的复杂性，允许程序通过文件描述符（File Descriptor） 的形式读写数据。
// 服务器端
// 1.创建 Socket ：socket() 函数。
// 2.绑定地址 ：bind() 绑定 IP 和端口。
// 3.监听连接 ：listen() 等待客户端。
// 4.接受连接 ：accept() 获取客户端连接。
// 5.收发数据 ：read()/write() 或 send()/recv()。
// 6.关闭连接 ：close()。
// 客户端
// 1.创建 Socket ：socket()。
// 2.连接服务器 ：connect()。
// 3.收发数据 ：send()/recv()。
// 4.关闭连接 ：close()。
class socket : noncopyable {
   public:
    explicit socket(int sockfd) : sockfd_(sockfd) {}
    ~socket();

    // 返回文件描述符（File Descriptor）
    int fd() const { return sockfd_; }
    // 绑定到具体地址，此处 cai::address 对IP 和端口进行了一个简单封装
    void bind_address(const cai::address &localaddr);
    // 监听连接
    void listen();
    // 接受连接
    int accept(cai::address *peeraddr);

    // 关闭发送方向（不再发送数据）
    void shutdown_write();

    // 设置 TCP_NODELAY 选项（禁用 Nagle 算法）
    void set_tcp_no_delay(bool on);
    // 设置 SO_REUSEADDR 选项（允许地址复用）
    void set_reuse_addr(bool on);
    // 设置 SO_REUSEPORT 选项（允许端口复用）
    void set_reuse_port(bool on);
    // 设置 SO_KEEPALIVE 选项（启用 TCP 保活机制）
    void set_keep_alive(bool on);

   private:
    // 底层文件描述符（File Descriptor）
    const int sockfd_;
};
}  // namespace cai
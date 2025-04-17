#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>

#include <string>

class address {
   public:
    explicit address(uint16_t port = 0, std::string ip = "127.0.0.1");
    explicit address(const sockaddr_in &addr) : addr_(addr) {}

    std::string to_ip() const;
    std::string to_ip_port() const;
    uint16_t to_port() const;

    const sockaddr_in *sock_addr() const { return &addr_; }
    void set_sock_addr(const sockaddr_in &addr) { addr_ = addr; }

   private:
    sockaddr_in addr_;
};
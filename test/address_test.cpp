#include "caimr/net/address.h"

#include <arpa/inet.h>
#include <string.h>

#include <cassert>
#include <iostream>

void test_default_constructor() {
    cai::address addr;
    assert(addr.to_port() == 0);
    assert(addr.to_ip() == "127.0.0.1");
    assert(addr.to_ip_port() == "127.0.0.1:0");
    std::cout << "test_default_constructor passed." << std::endl;
}

void test_custom_ip_port() {
    cai::address addr(8080, "192.168.1.1");
    assert(addr.to_port() == 8080);
    assert(addr.to_ip() == "192.168.1.1");
    assert(addr.to_ip_port() == "192.168.1.1:8080");
    std::cout << "test_custom_ip_port passed." << std::endl;
}

void test_sockaddr_in_constructor() {
    sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(1234);
    inet_pton(AF_INET, "10.0.0.1", &sa.sin_addr);

    cai::address addr(sa);
    assert(addr.to_port() == 1234);
    assert(addr.to_ip() == "10.0.0.1");
    assert(addr.to_ip_port() == "10.0.0.1:1234");
    std::cout << "test_sockaddr_in_constructor passed." << std::endl;
}

int main() {
    test_default_constructor();
    test_custom_ip_port();
    test_sockaddr_in_constructor();

    std::cout << "all tests passed!" << std::endl;
    return 0;
}
#pragma once

#include "caimr/common/noncopyable.h"

class address;

namespace cai {
class socket : noncopyable {
   public:
    explicit socket(int sockfd) : sockfd_(sockfd) {}
    ~socket();

    int fd() const { return sockfd_; }
    void bind_address(const address &localaddr);
    void listen();
    int accept(address *peeraddr);

    void shutdown_write();

    void set_tcp_no_delay(bool on);
    void set_reuse_addr(bool on);
    void set_reuse_port(bool on);
    void set_keep_alive(bool on);

   private:
    const int sockfd_;
};
}  // namespace cai
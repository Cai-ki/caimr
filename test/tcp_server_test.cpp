#include "caimr/net/tcp_server.h"

#include <string>

#include "caimr/logger/logging.h"
#include "caimr/net/address.h"
#include "caimr/net/callbacks.h"

using namespace cai;

class echo_server {
   public:
    echo_server(eloop *loop, const address &addr, const std::string &name)
        : server_(loop, addr, name), loop_(loop) {
        server_.set_conn_callback(
            std::bind(&echo_server::on_connnect, this, std::placeholders::_1));

        server_.set_msg_callback(
            std::bind(&echo_server::on_message, this, std::placeholders::_1,
                      std::placeholders::_2, std::placeholders::_3));

        server_.set_thread_num(3);
    }
    void start() { server_.start(); }

   private:
    void on_connnect(const tcp_conn_ptr &conn) {
        if (conn->is_connected()) {
            LOG_INFOF("connection up : %s",
                      conn->peer_address().to_ip_port().c_str());
        } else {
            LOG_INFOF("connection down : %s",
                      conn->peer_address().to_ip_port().c_str());
        }
    }

    void on_message(const tcp_conn_ptr &conn, buffer *buf, timestamp time) {
        std::string msg = buf->retrieve_all_as_string();
        conn->send(msg);
    }

    eloop *loop_;
    tcp_server server_;
};

int main() {
    eloop loop;
    address addr(8888);
    echo_server server(&loop, addr, "echo_server");
    server.start();
    loop.loop();
    return 0;
}
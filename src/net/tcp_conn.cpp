#include "net/tcp_conn.h"

#include <netinet/tcp.h>
#include <sys/socket.h>

#include <cerrno>

#include "logger/logging.h"
#include "net/address.h"
#include "net/chan.h"
#include "net/eloop.h"
#include "net/socket.h"

namespace cai {
namespace {
eloop* check_loop_not_null(eloop* loop) {
    if (!loop) {
        LOG_FATALF("invalid event loop pointer");
    }
    return loop;
}
}  // namespace

tcp_conn::tcp_conn(eloop* loop, const uint32_t id, const std::string& name,
                   int sockfd, const address& local_addr,
                   const address& peer_addr)
    : loop_(check_loop_not_null(loop)),
      id_(id),
      name_(name),
      state_(CONNENTING),
      reading_(true),
      socket_(new socket(sockfd)),
      ch_(new chan(loop, sockfd)),
      local_addr_(local_addr),
      peer_addr_(peer_addr),
      high_water_mark_(64 * 1024 * 1024) {
    ch_->set_read_callback(
        std::bind(&tcp_conn::handle_read, this, std::placeholders::_1));
    ch_->set_write_callback(std::bind(&tcp_conn::handle_write, this));
    ch_->set_close_callback(std::bind(&tcp_conn::handle_close, this));
    ch_->set_error_callback(std::bind(&tcp_conn::handle_error, this));

    LOG_INFOF("tcp_conn created [%s] fd=%d", name_.c_str(), sockfd);
    socket_->set_keep_alive(true);
}

tcp_conn::~tcp_conn() {
    LOG_INFOF("tcp_conn destroyed [%s] fd=%d state=%d", name_.c_str(),
              ch_->fd(), static_cast<int>(state_.load()));
}

void tcp_conn::send(const std::string& data) {
    if (state_.load() == CONNECTED) {
        if (loop_->is_in_loop_thread()) {
            send_in_loop(data.data(), data.size());
        } else {
            loop_->run_in_loop(std::bind(&tcp_conn::send_in_loop, this,
                                         data.data(), data.size()));
        }
    }
}

void tcp_conn::send_in_loop(const void* data, size_t len) {
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool fault_occurred = false;

    if (state_.load() == DISCONNECTED) {
        LOG_WARNF("conn already closed, abort sending");
        return;
    }

    if (!ch_->is_writing() && output_buf_.readable_bytes() == 0) {
        nwrote = ::write(ch_->fd(), data, len);
        if (nwrote >= 0) {
            remaining = len - nwrote;
            if (remaining == 0 && write_complete_callback_) {
                loop_->queue_in_loop(
                    std::bind(write_complete_callback_, shared_from_this()));
            }
        } else {
            if (errno != EWOULDBLOCK) {
                if (errno == EPIPE || errno == ECONNRESET) {
                    fault_occurred = true;
                }
                LOG_ERRORF("write error: %s", strerror(errno));
            }
            nwrote = 0;
        }
    }

    if (!fault_occurred && remaining > 0) {
        size_t old_len = output_buf_.readable_bytes();
        if (old_len + remaining >= high_water_mark_ &&
            old_len < high_water_mark_ && high_water_mark_callback_) {
            loop_->queue_in_loop(std::bind(high_water_mark_callback_,
                                           shared_from_this(),
                                           old_len + remaining));
        }
        output_buf_.append(static_cast<const char*>(data) + nwrote, remaining);
        if (!ch_->is_writing()) {
            ch_->enable_writing();
        }
    }
}

void tcp_conn::shutdown() {
    if (state_.load() == CONNECTED) {
        set_state(DISCONNENTING);
        loop_->run_in_loop(std::bind(&tcp_conn::shutdown_in_loop, this));
    }
}

void tcp_conn::shutdown_in_loop() {
    if (!ch_->is_writing()) {
        socket_->shutdown_write();
    }
}

void tcp_conn::establish_conn() {
    set_state(CONNECTED);
    ch_->tie(shared_from_this());
    ch_->enable_reading();

    if (conn_callback_) {
        conn_callback_(shared_from_this());
    }
}

void tcp_conn::destroy_conn() {
    if (state_.load() == CONNECTED) {
        set_state(DISCONNECTED);
        ch_->disable_all();
        if (conn_callback_) {
            conn_callback_(shared_from_this());
        }
    }
    ch_->remove();
}

void tcp_conn::handle_read(time::time_point receive_time) {
    int saved_errno = 0;
    ssize_t n = input_buf_.read_fd(ch_->fd(), &saved_errno);
    if (n > 0) {
        if (msg_callback_) {
            msg_callback_(shared_from_this(), &input_buf_, receive_time);
        }
    } else if (n == 0) {
        handle_close();
    } else {
        errno = saved_errno;
        LOG_ERRORF("read error: %s", strerror(errno));
        handle_error();
    }
}

void tcp_conn::handle_write() {
    if (ch_->is_writing()) {
        int saved_errno = 0;
        ssize_t n = output_buf_.write_fd(ch_->fd(), &saved_errno);
        if (n > 0) {
            output_buf_.retrieve(n);
            if (output_buf_.readable_bytes() == 0) {
                ch_->disable_writing();
                if (write_complete_callback_) {
                    loop_->queue_in_loop(std::bind(write_complete_callback_,
                                                   shared_from_this()));
                }
                if (state_.load() == DISCONNENTING) {
                    shutdown_in_loop();
                }
            }
        } else {
            LOG_ERRORF("write error: %s", strerror(saved_errno));
        }
    } else {
        LOG_WARNF("conn fd=%d is closing", ch_->fd());
    }
}

void tcp_conn::handle_close() {
    LOG_INFOF("closing conn [%s] fd=%d", name_.c_str(), ch_->fd());
    set_state(DISCONNECTED);
    ch_->disable_all();

    auto self = shared_from_this();
    if (conn_callback_) {
        conn_callback_(self);
    }
    if (close_callback_) {
        close_callback_(self);
    }
}

void tcp_conn::handle_error() {
    int err = 0;
    socklen_t errlen = sizeof(err);
    if (::getsockopt(ch_->fd(), SOL_SOCKET, SO_ERROR, &err, &errlen) < 0) {
        err = errno;
    }
    LOG_ERRORF("conn [%s] error: %s", name_.c_str(), strerror(err));
}
}  // namespace cai
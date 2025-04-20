#pragma once

#include <functional>
#include <memory>

namespace cai {
class buffer;
class tcp_conn;
class timestamp;

using tcp_conn_ptr = std::shared_ptr<tcp_conn>;
using conn_callback = std::function<void(const tcp_conn_ptr &)>;
using close_callback = std::function<void(const tcp_conn_ptr &)>;
using write_complete_callback = std::function<void(const tcp_conn_ptr &)>;
using high_water_mark_callback =
    std::function<void(const tcp_conn_ptr &, size_t)>;

using msg_callback =
    std::function<void(const tcp_conn_ptr &, buffer *, timestamp)>;
}  // namespace cai
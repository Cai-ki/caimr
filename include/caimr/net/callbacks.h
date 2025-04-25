#pragma once

#include <functional>
#include <memory>

#include "caimr/common/time.h"

namespace cai {
class buffer;
class tcp_conn;

using tcp_conn_ptr = std::shared_ptr<tcp_conn>;
using conn_callback = std::function<void(const tcp_conn_ptr &)>;
using close_callback = std::function<void(const tcp_conn_ptr &)>;
using write_complete_callback = std::function<void(const tcp_conn_ptr &)>;
using high_water_mark_callback =
    std::function<void(const tcp_conn_ptr &, size_t)>;

using msg_callback =
    std::function<void(const tcp_conn_ptr &, buffer *, time::time_point)>;
}  // namespace cai
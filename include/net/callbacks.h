#pragma once

#include <functional>
#include <memory>

#include "../common/time.h"

namespace cai {
class buffer;
class tcp_conn;

using tcp_conn_ptr = std::shared_ptr<tcp_conn>;
// 建立连接时的回调
using conn_callback = std::function<void(const tcp_conn_ptr &)>;
// 关闭连接时的回调
using close_callback = std::function<void(const tcp_conn_ptr &)>;
// 数据写完后的回调
using write_complete_callback = std::function<void(const tcp_conn_ptr &)>;
// 高水位标记，当发送数据的缓冲区占用量超过高水位时触发的回调
using high_water_mark_callback =
    std::function<void(const tcp_conn_ptr &, size_t)>;
// 读取数据后的回调
using msg_callback =
    std::function<void(const tcp_conn_ptr &, buffer *, time::time_point)>;
}  // namespace cai
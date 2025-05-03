#pragma once

#include <unordered_map>
#include <vector>

#include "../common/noncopyable.h"
#include "../common/time.h"

namespace cai {
class chan;
class eloop;

// 接口，本身作为一个抽象提供poller的基本能力，主要提供对chan的管辖
// chan是对fd的抽象，poller同理是对底层select poll epoll的抽象封装
class poller : noncopyable {
   public:
    using chan_list = std::vector<chan*>;

    poller(eloop* loop);
    virtual ~poller() = default;

    virtual time::time_point poll(int timeout_ms, chan_list* active_chs) = 0;
    virtual void update_chan(chan* ch) = 0;
    virtual void remove_chan(chan* ch) = 0;

    bool has_chan(chan* ch) const;

    // 构造不同的poller
    static poller* new_default_poller(eloop* loop);

   protected:
    // 允许派生类访问
    using chanmap = std::unordered_map<int, chan*>;
    chanmap chs_;

   private:
    // 私有变量，不允许访问
    eloop* owner_loop_;
};
}  // namespace cai
#include "net/poller.h"

#include "net/chan.h"

namespace cai {
poller::poller(eloop* loop) : owner_loop_(loop) {}

bool poller::has_chan(chan* ch) const {
    auto it = chs_.find(ch->fd());
    return it != chs_.end() && it->second == ch;
}
}  // namespace cai
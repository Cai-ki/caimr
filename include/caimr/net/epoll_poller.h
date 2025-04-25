#pragma once

#include <sys/epoll.h>

#include <vector>

#include "caimr/common/time.h"
#include "caimr/net/poller.h"

namespace cai {
class chan;

class epoll_poller : public poller {
   public:
    explicit epoll_poller(eloop* loop);
    ~epoll_poller() override;

    time::time_point poll(int timeout_ms, chanlist* active_chs) override;
    void update_chan(chan* ch) override;
    void remove_chan(chan* ch) override;

   private:
    static constexpr int INIT_EVENT_LIST_SIZE = 16;

    void fill_active_chs(int num_events, chanlist* active_chs) const;
    void update(int operation, chan* chan);

    using eventlist = std::vector<epoll_event>;

    int epollfd_;
    eventlist events_;
};
}  // namespace cai
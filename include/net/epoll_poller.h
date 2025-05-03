#pragma once

#include <sys/epoll.h>

#include <vector>

#include "../common/time.h"
#include "./poller.h"

namespace cai {
class chan;

// poller接口的epoll实现
class epoll_poller : public poller {
   public:
    explicit epoll_poller(eloop* loop);
    ~epoll_poller() override;

    // 具体的实现，核心方法poll，传入返回触发事件的空列表
    time::time_point poll(int timeout_ms, chan_list* active_chs) override;
    void update_chan(chan* ch) override;
    void remove_chan(chan* ch) override;

   private:
    // 具体实现的私有属性和方法

    // eventlist初始大小
    static constexpr int INIT_EVENT_LIST_SIZE = 16;
    // 向chan_list*中填入触发的chan*
    void fill_active_chs(int num_events, chan_list* active_chs) const;
    void update(int operation, chan* chan);

    using eventlist = std::vector<epoll_event>;

    // epoll的fd
    int epollfd_;
    // 类似于缓冲区，epoll拉取触发的事件会放入到这里，并返回个数。相比于C风格的数组，动态数组可以扩容对于大量事件触发情况更友好
    eventlist events_;
};
}  // namespace cai
#include "net/epoll_poller.h"

#include <sys/epoll.h>
#include <unistd.h>

#include "log/logging.h"
#include "net/chan.h"

namespace cai {
constexpr int NEW = -1;
constexpr int ADDED = 1;
constexpr int DELETED = 2;

epoll_poller::epoll_poller(eloop* loop)
    : poller(loop),
      epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
      events_(INIT_EVENT_LIST_SIZE) {
    if (epollfd_ < 0) {
        LOG_FATALF("epoll_create error: %d", errno);
    }
}

epoll_poller::~epoll_poller() { ::close(epollfd_); }

time::time_point epoll_poller::poll(int timeout_ms, chan_list* active_chs) {
    int num_events = ::epoll_wait(epollfd_, &*events_.begin(),
                                  static_cast<int>(events_.size()), timeout_ms);
    time::time_point now(time::clock::now());
    if (num_events > 0) {
        fill_active_chs(num_events, active_chs);
        if (num_events == events_.size()) {
            events_.resize(events_.size() * 2);
        }
    }
    return now;
}

void epoll_poller::fill_active_chs(int num_events,
                                   chan_list* active_chs) const {
    for (int i = 0; i < num_events; ++i) {
        chan* ch = static_cast<chan*>(events_[i].data.ptr);
        ch->set_revents(events_[i].events);
        active_chs->push_back(ch);
    }
}

void epoll_poller::update_chan(chan* chan) {
    const int index = chan->index();
    if (index == NEW || index == DELETED) {
        int fd = chan->fd();
        if (index == NEW) {
            chs_[fd] = chan;
        }
        chan->set_index(ADDED);
        update(EPOLL_CTL_ADD, chan);
    } else {
        update(EPOLL_CTL_MOD, chan);
    }
}

void epoll_poller::remove_chan(chan* chan) {
    int fd = chan->fd();
    chs_.erase(fd);
    chan->set_index(DELETED);
    update(EPOLL_CTL_DEL, chan);
}

void epoll_poller::update(int operation, chan* ch) {
    epoll_event event{};
    event.events = ch->events();
    event.data.ptr = ch;
    int fd = ch->fd();
    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
        LOG_ERRORF("epoll_ctl op:%d fd:%d error:%d", operation, fd, errno);
    }
}
}  // namespace cai
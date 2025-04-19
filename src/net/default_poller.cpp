#include <stdlib.h>

#include "caimr/net/epoll_poller.h"
#include "caimr/net/poller.h"

namespace cai {
poller *poller::new_default_poller(eloop *loop) {
    if (::getenv("CAI_USE_POLL")) {
        return nullptr;
    } else {
        return new epoll_poller(loop);
    }
}
}  // namespace cai
#pragma once

#include <unordered_map>
#include <vector>

#include "../common/noncopyable.h"
#include "../common/time.h"

namespace cai {
class chan;
class eloop;

class poller : noncopyable {
   public:
    using chan_list = std::vector<chan*>;

    poller(eloop* loop);
    virtual ~poller() = default;

    virtual time::time_point poll(int timeout_ms, chan_list* active_chs) = 0;
    virtual void update_chan(chan* ch) = 0;
    virtual void remove_chan(chan* ch) = 0;

    bool has_chan(chan* ch) const;

    static poller* new_default_poller(eloop* loop);

   protected:
    using chanmap = std::unordered_map<int, chan*>;
    chanmap chs_;

   private:
    eloop* owner_loop_;
};
}  // namespace cai
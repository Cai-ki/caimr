#pragma once

#include <unordered_map>
#include <vector>

#include "caimr/common/noncopyable.h"
#include "caimr/common/timestamp.h"

namespace cai {
class chan;
class eloop;

class poller : noncopyable {
   public:
    using chanlist = std::vector<chan*>;

    poller(eloop* loop);
    virtual ~poller() = default;

    virtual timestamp poll(int timeout_ms, chanlist* active_chs) = 0;
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
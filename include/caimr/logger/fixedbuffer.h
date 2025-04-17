#pragma once

#include <string.h>
#include <strings.h>

#include <string>

#include "caimr/common/noncopyable.h"

namespace cai {
constexpr int KB_BUFFER_SIZE = 1024;
constexpr int MB_BUFFER_SIZE = 1048576;

template <int SIZE>
class fixedbuffer : noncopyable {
   public:
    fixedbuffer() : cur_(data_) {}

    void append(const char* buf, size_t len) {
        if (static_cast<size_t>(size()) > len) {
            memcpy(cur_, buf, len);
            cur_ += len;
        }
    }

    const char* data() const { return data_; }
    int cap() const { return static_cast<int>(end() - data_); }
    int size() const { return static_cast<int>(end() - cur_); }
    int used() const { return static_cast<int>(cur_ - data_); }

    char* cur() { return cur_; }
    void add(size_t len) { cur_ += len; }

    void reset() { cur_ = data_; }
    void bzero() { ::bzero(data_, sizeof(data_)); }

    std::string to_string() const { return std::string(data_, used()); }

   private:
    const char* end() const { return data_ + sizeof(data_); }

    char data_[SIZE];
    char* cur_;
};
}  // namespace cai
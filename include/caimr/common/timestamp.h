#pragma once

#include <sys/time.h>

#include <string>

class timestamp {
   public:
    timestamp() : microseconds_(0) {}
    explicit timestamp(int64_t microseconds) : microseconds_(microseconds) {}
    static timestamp now();
    std::string to_string() const;
    int64_t microseconds() const { return microseconds_; }

    static const int microsecond = 1;
    static const int millisecond = 1000;
    static const int second = 1000000;

   private:
    int64_t microseconds_;
};

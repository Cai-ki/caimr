#include "caimr/common/timestamp.h"

#include <stdio.h>
#include <time.h>

namespace cai {
timestamp timestamp::now() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return timestamp(tv.tv_sec * second + tv.tv_usec);
}

std::string timestamp::to_string() const {
    char buf[64] = {0};
    time_t seconds = microseconds_ / second;
    tm* tm_time = localtime(&seconds);
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
             tm_time->tm_year + 1900, tm_time->tm_mon + 1, tm_time->tm_mday,
             tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec);
    return buf;
}
}  // namespace cai
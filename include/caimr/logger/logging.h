#pragma once

#include <string.h>
#include <sys/time.h>

#include <functional>

#include "caimr/common/timestamp.h"
#include "caimr/logger/logstream.h"

namespace cai {
class sfile {
   public:
    explicit sfile(const char *filename) : data_(filename) {
        const char *slash = strrchr(filename, '/');
        if (slash) {
            data_ = slash + 1;
        }
        size_ = static_cast<int>(strlen(data_));
    }

    const char *data_;
    int size_;
};

class logger {
   public:
    enum loglevel {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        LEVEL_COUNT,
    };

    logger(const char *file, int line);
    logger(const char *file, int line, loglevel level);
    logger(const char *file, int line, loglevel level, const char *func);
    ~logger();

    logstream &stream() { return impl_.stream_; }

    static loglevel level();
    static void set_level(loglevel level);

    using ofunc = std::function<void(const char *msg, int len)>;
    using ffunc = std::function<void()>;
    static void set_output(ofunc);
    static void set_flush(ffunc);

   private:
    class impl {
       public:
        using loglevel = logger::loglevel;
        impl(loglevel level, int errno_, const char *file, int line);
        void format_time();
        void finish();

        timestamp time_;
        logstream stream_;
        loglevel level_;
        int line_;
        sfile basename_;
    };

    impl impl_;
};

extern logger::loglevel g_loglevel;

inline logger::loglevel loglevel() { return g_loglevel; }

const char *errno_msg(int errno_);
}  // namespace cai

#define LOG_DEBUG                                      \
    if (cai::logger::loglevel() <= cai::logger::DEBUG) \
    cai::logger(__FILE__, __LINE__, cai::logger::DEBUG, __func__).stream()
#define LOG_INFO                                      \
    if (cai::logger::loglevel() <= cai::logger::INFO) \
    cai::logger(__FILE__, __LINE__).stream()
#define LOG_WARN cai::logger(__FILE__, __LINE__, cai::logger::WARN).stream()
#define LOG_ERROR cai::logger(__FILE__, __LINE__, cai::logger::ERROR).stream()
#define LOG_FATAL cai::logger(__FILE__, __LINE__, cai::logger::FATAL).stream()

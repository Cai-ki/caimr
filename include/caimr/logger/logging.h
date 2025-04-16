#pragma once

#include <string.h>
#include <sys/time.h>

#include <functional>

#include "caimr/common/timestamp.h"
#include "caimr/logger/logstream.h"

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

#define LOG_DEBUG                    \
    if (loglevel() <= logger::DEBUG) \
    logger(__FILE__, __LINE__, logger::DEBUG, __func__).stream()
#define LOG_INFO \
    if (loglevel() <= logger::INFO) logger(__FILE__, __LINE__).stream()
#define LOG_WARN logger(__FILE__, __LINE__, logger::WARN).stream()
#define LOG_ERROR logger(__FILE__, __LINE__, logger::ERROR).stream()
#define LOG_FATAL logger(__FILE__, __LINE__, logger::FATAL).stream()

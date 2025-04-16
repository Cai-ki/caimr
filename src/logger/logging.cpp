#include "caimr/logger/logging.h"

#include <time.h>

#include <cstring>

#include "caimr/thread/cthread.h"

namespace thread_info {
__thread char errnobuf_[512];
__thread char time_[64];
__thread time_t last_second_;
};  // namespace thread_info

const char* errno_msg(int errno_) {
    return strerror_r(errno_, thread_info::errnobuf_,
                      sizeof(thread_info::errnobuf_));
}

const char* level_name[logger::loglevel::LEVEL_COUNT]{
    " [TRACE] ", " [DEBUG] ", " [INFO] ", " [WARN] ", " [ERROR] ", " [FATAL] ",
};

const int level_name_len[logger::loglevel::LEVEL_COUNT]{9, 9, 8, 8, 9, 9};

logger::loglevel initloglevel() { return logger::INFO; }

logger::loglevel g_loglevel = initloglevel();

static void default_output(const char* data, int len) {
    fwrite(data, len, sizeof(char), stdout);
}

static void default_flush() { fflush(stdout); }

logger::ofunc g_output = default_output;
logger::ffunc g_flush = default_flush;

logger::impl::impl(logger::loglevel level, int errno_, const char* file,
                   int line)
    : time_(timestamp::now()),
      stream_(),
      level_(level),
      line_(line),
      basename_(file) {
    format_time();
    stream_ << gtemplate(level_name[level], level_name_len[level]);

    if (errno_ != 0) {
        stream_ << errno_msg(errno_) << " (errno=" << errno_ << ") ";
    }
}

void logger::impl::format_time() {
    timestamp now = timestamp::now();
    time_t seconds =
        static_cast<time_t>(now.microseconds() / timestamp::second);
    int microseconds = static_cast<int>(now.microseconds() % timestamp::second);
    memcpy(thread_info::time_, now.to_string().c_str(), now.to_string().size());
    thread_info::last_second_ = seconds;
    stream_ << gtemplate(thread_info::time_, 19);
}

void logger::impl::finish() {
    stream_ << " - " << gtemplate(basename_.data_, basename_.size_) << ':'
            << line_ << '\n';
}

logger::logger(const char* file, int line) : impl_(INFO, 0, file, line) {}

logger::logger(const char* file, int line, logger::loglevel level)
    : impl_(level, 0, file, line) {}

logger::logger(const char* file, int line, logger::loglevel level,
               const char* func)
    : impl_(level, 0, file, line) {
    impl_.stream_ << func << ' ';
}

logger::~logger() {
    impl_.finish();
    const logstream::fbuffer& buf(stream().buffer());
    g_output(buf.data(), buf.used());
    if (impl_.level_ == FATAL) {
        g_flush();
        abort();
    }
}

void logger::set_level(logger::loglevel level) { g_loglevel = level; }

void logger::set_output(ofunc out) { g_output = out; }

void logger::set_flush(ffunc flush) { g_flush = flush; }
#pragma once

#include <string>

#include "../common/noncopyable.h"
#include "./fixedbuffer.h"

namespace cai {
class gtemplate : noncopyable {
   public:
    gtemplate() : data_(nullptr), len_(0) {}

    explicit gtemplate(const char* data, int len) : data_(data), len_(len) {}

    const char* data_;
    int len_;
};

class logstream : noncopyable {
   public:
    using fbuffer = fixedbuffer<KB_BUFFER_SIZE>;

    void append(const char* data, int len) { buffer_.append(data, len); }
    const fbuffer& buffer() const { return buffer_; }
    void reset() { buffer_.reset(); }

    logstream& operator<<(short);
    logstream& operator<<(unsigned short);
    logstream& operator<<(int);
    logstream& operator<<(unsigned int);
    logstream& operator<<(long);
    logstream& operator<<(unsigned long);
    logstream& operator<<(long long);
    logstream& operator<<(unsigned long long);

    logstream& operator<<(float v);
    logstream& operator<<(double v);

    logstream& operator<<(char c);
    logstream& operator<<(const void* data);
    logstream& operator<<(const char* str);
    logstream& operator<<(const unsigned char* str);
    logstream& operator<<(const std::string& str);
    logstream& operator<<(const fbuffer& buf);

    logstream& operator<<(const gtemplate& g);

   private:
    static constexpr int MAX_NUM_SIZE = 48;

    template <typename T>
    void format_integer(T);

    fbuffer buffer_;
};
}  // namespace cai
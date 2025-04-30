#include "logger/logstream.h"

#include <algorithm>

namespace cai {
static constexpr char digits[] = {'9', '8', '7', '6', '5', '4', '3',
                                  '2', '1', '0', '1', '2', '3', '4',
                                  '5', '6', '7', '8', '9'};

template <typename T>
void logstream::format_integer(T num) {
    if (buffer_.size() >= MAX_NUM_SIZE) {
        char* start = buffer_.cur();
        char* cur = start;
        const char* zero = digits + 9;
        bool negative = (num < 0);

        do {
            int remainder = static_cast<int>(num % 10);
            *(cur++) = zero[remainder];
            num = num / 10;
        } while (num != 0);

        if (negative) {
            *(cur++) = '-';
        }

        std::reverse(start, cur);
        buffer_.add(static_cast<int>(cur - start));
    }
}

logstream& logstream::operator<<(short v) {
    *this << static_cast<int>(v);
    return *this;
}

logstream& logstream::operator<<(unsigned short v) {
    *this << static_cast<unsigned int>(v);
    return *this;
}

logstream& logstream::operator<<(int v) {
    format_integer(v);
    return *this;
}

logstream& logstream::operator<<(unsigned int v) {
    format_integer(v);
    return *this;
}

logstream& logstream::operator<<(long v) {
    format_integer(v);
    return *this;
}

logstream& logstream::operator<<(unsigned long v) {
    format_integer(v);
    return *this;
}

logstream& logstream::operator<<(long long v) {
    format_integer(v);
    return *this;
}

logstream& logstream::operator<<(unsigned long long v) {
    format_integer(v);
    return *this;
}

logstream& logstream::operator<<(float v) {
    *this << static_cast<double>(v);
    return *this;
}

logstream& logstream::operator<<(double v) {
    if (buffer_.size() >= MAX_NUM_SIZE) {
        char buf[32];
        int len = snprintf(buffer_.cur(), MAX_NUM_SIZE, "%.12g", v);
        buffer_.add(len);
    }
    return *this;
}

logstream& logstream::operator<<(char c) {
    buffer_.append(&c, 1);
    return *this;
}

logstream& logstream::operator<<(const void* data) {
    *this << static_cast<const char*>(data);
    return *this;
}

logstream& logstream::operator<<(const char* str) {
    if (str) {
        buffer_.append(str, strlen(str));
    } else {
        buffer_.append("(null)", 6);
    }
    return *this;
}

logstream& logstream::operator<<(const unsigned char* str) {
    return operator<<(reinterpret_cast<const char*>(str));
}

logstream& logstream::operator<<(const std::string& str) {
    buffer_.append(str.c_str(), str.size());
    return *this;
}

logstream& logstream::operator<<(const fbuffer& buf) {
    *this << buf.to_string();
    return *this;
}

logstream& logstream::operator<<(const gtemplate& g) {
    buffer_.append(g.data_, g.len_);
    return *this;
}
}  // namespace cai
#pragma once

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

namespace cai {
class buffer {
   public:
    static constexpr size_t CHEAP_PREPEND = 8;
    static constexpr size_t INITIAL_SIZE = 1024;

    explicit buffer(size_t initial_size = INITIAL_SIZE)
        : buf_(CHEAP_PREPEND + initial_size),
          reader_index_(CHEAP_PREPEND),
          writer_index_(CHEAP_PREPEND) {}

    size_t readable_bytes() const { return writer_index_ - reader_index_; }
    size_t writable_bytes() const { return buf_.size() - writer_index_; }
    size_t prependable_bytes() const { return reader_index_; }

    const char* peek() const { return begin() + reader_index_; }

    void retrieve(size_t len) {
        if (len < readable_bytes()) {
            reader_index_ += len;
        } else {
            retrieve_all();
        }
    }

    void retrieve_all() {
        reader_index_ = CHEAP_PREPEND;
        writer_index_ = CHEAP_PREPEND;
    }

    std::string retrieve_all_as_string() {
        return retrieve_as_string(readable_bytes());
    }

    std::string retrieve_as_string(size_t len) {
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    void ensure_writable_bytes(size_t len) {
        if (writable_bytes() < len) make_space(len);
    }

    void append(const char* data, size_t len) {
        ensure_writable_bytes(len);
        std::copy(data, data + len, begin_write());
        writer_index_ += len;
    }

    char* begin_write() { return begin() + writer_index_; }
    const char* begin_write() const { return begin() + writer_index_; }

    ssize_t read_fd(int fd, int* save_errno);
    ssize_t write_fd(int fd, int* save_errno);

   private:
    char* begin() { return buf_.data(); }
    const char* begin() const { return buf_.data(); }

    void make_space(size_t len) {
        if (writable_bytes() + prependable_bytes() < len + CHEAP_PREPEND) {
            buf_.resize(writer_index_ + len);
        } else {
            size_t readable = readable_bytes();
            std::copy_backward(
                begin() + reader_index_, begin() + writer_index_,
                begin() +
                    CHEAP_PREPEND);  // 从后向前复制，std::copy()面对结果重叠部分可能导致未定义行为
            reader_index_ = CHEAP_PREPEND;
            writer_index_ = reader_index_ + readable;
        }
    }

    std::vector<char> buf_;
    size_t reader_index_;
    size_t writer_index_;
};
}  // namespace cai
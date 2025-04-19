#include "caimr/net/buffer.h"

#include <sys/uio.h>
#include <unistd.h>

#include <cerrno>

namespace cai {
ssize_t buffer::read_fd(int fd, int* save_errno) {
    char extrabuf[65536] = {0};
    struct iovec vec[2];

    const size_t writable = writable_bytes();
    vec[0].iov_base = begin_write();
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);

    const int iovcnt = (writable < sizeof(extrabuf)) ? 2 : 1;
    const ssize_t n = ::readv(fd, vec, iovcnt);

    if (n < 0) {
        *save_errno = errno;
    } else if (n <= writable) {
        writer_index_ += n;
    } else {
        writer_index_ = buf_.size();
        append(extrabuf, n - writable);
    }
    return n;
}

ssize_t buffer::write_fd(int fd, int* save_errno) {
    ssize_t n = ::write(fd, peek(), readable_bytes());
    if (n < 0) {
        *save_errno = errno;
    }
    return n;
}
}  // namespace cai
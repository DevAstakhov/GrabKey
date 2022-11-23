#include "fd_reader.h"
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include "fd_monitor.h"

namespace keyboard {

FdReader::FdReader(int fd) : fd(fd), poller(fd) {
    int flags = ::fcntl(fd, F_GETFL, 0);
    if (flags < 0)
        throw std::runtime_error(strerror(errno));

    if (::fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
        throw std::runtime_error(strerror(errno));
}

void FdReader::dispose() {
    this->poller.dispose();
}

std::vector<char>& FdReader::read_to(
    std::vector<char>& out_buffer, size_t read_bytes, BufferPolicy policy
) const {
    char* buf = NULL;

    if (policy == Rewrite)
        out_buffer.clear();

    const auto sz = out_buffer.size();
    out_buffer.resize(sz + read_bytes);
    buf = out_buffer.data() + sz;

    ssize_t bytes = ::read(fd, buf, read_bytes);

    const bool has_data = !(bytes == -1 && errno == EAGAIN);
    if (has_data)
        out_buffer.resize(sz + bytes);

    if (bytes < 0)
        throw std::runtime_error(strerror(errno));

    return out_buffer;
}

std::vector<char> FdReader::read(size_t read_bytes) {
    std::vector<char> out_buffer;

    read_to(out_buffer, read_bytes, Rewrite);

    return out_buffer;
}

PollResult FdReader::poll(int timeout_ms) const {
    return poller.poll(timeout_ms);
}
void FdReader::interrupt() const {
    poller.interrupt();
}

void FdReader::clear() const {
    if (::tcflush(fd, TCIOFLUSH) < 0)
        throw std::runtime_error(strerror(errno));
}

}  // namespace keyboard
#pragma once

#include "fd_poller.h"
#include <memory>
#include <vector>

namespace keyboard {

class FdReader {
    int fd;
    FdPoller poller;

public:
    FdReader(int fd);
    void dispose();

    enum BufferPolicy {
        Append,
        Rewrite
    };

    std::vector<char>& read_to(std::vector<char>& out_buffer, size_t read_bytes, BufferPolicy policy) const;
    std::vector<char> read(size_t read_bytes);

    PollResult poll(int timeout_ms) const;
    void interrupt() const;
    void clear() const;
};

}
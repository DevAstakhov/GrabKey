#pragma once

#include <chrono>
#include <memory>
#include <optional>
#include <vector>
#include "fd_poller.h"

namespace keyboard {

class FdReader {
    int fd;
    int flags;
    FdPoller poller;

public:
    FdReader(int fd);
    void dispose();

    enum BufferPolicy {
        Append,
        Rewrite
    };

    std::vector<char>& read_to(
        std::vector<char>& out_buffer, size_t read_bytes, BufferPolicy policy
    ) const;
    std::vector<char> read(size_t read_bytes);

    PollResult poll(
        std::optional<std::chrono::milliseconds> timeout = std::nullopt
    ) const;

    void interrupt() const;
    void clear() const;
};

}  // namespace keyboard
#pragma once

#include "fd_monitor.h"
#include <memory>

namespace keyboard {

enum class PollResult {
    Timeout = -1,
    DataReady = 0,
    Interrupted = 1
};

class FdPoller {
    std::shared_ptr<monitor_ctx_t> the_ctx;

public:
    FdPoller(int fd);

    PollResult poll(int timeout_ms) const;
    void interrupt() const;
    void dispose();
};

}
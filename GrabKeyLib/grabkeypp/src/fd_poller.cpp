#include "fd_poller.h"
#include "fd_monitor.h"
#include <cstring>
#include <stdexcept>
#include <unistd.h>

namespace keyboard {

    FdPoller::FdPoller(int fd)
        : the_ctx(fd_monitor_create(fd),
            [](auto& ctx){
                fd_monitor_interrupt(ctx);
                fd_monitor_free(ctx);
            })
    {}

    static inline PollResult Process(poll_result result) {
        switch (result) {
            case error : throw std::runtime_error(strerror(errno));
            case timeout : return PollResult::Timeout;
            case interrupted : return PollResult::Interrupted;
            case data_ready : return PollResult::DataReady;
        }
    }

    PollResult FdPoller::poll(int timeout_ms) const
    {
        return Process(fd_monitor_poll(the_ctx.get(), timeout_ms));
    }

    void FdPoller::interrupt() const { fd_monitor_interrupt(the_ctx.get()); }    
    void FdPoller::dispose() { the_ctx.reset(); }

} // namespace keyboard
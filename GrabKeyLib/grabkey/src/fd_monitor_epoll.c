#include <stdint.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include "fd_monitor.h"

struct epoll_monitor_context_t {
    int fd;
    int epoll_fd;
    int interrupt_fd;
};

struct monitor_ctx_t* fd_monitor_create(int fd) {
    int epoll_fd = epoll_create(1);
    if (epoll_fd == -1)
        return 0;

    struct epoll_event kbd_event;
    kbd_event.events  = EPOLLIN;
    kbd_event.data.fd = fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &kbd_event))
        return 0;

    int interrupt_fd = eventfd(0, 0);
    if (interrupt_fd == -1)
        return 0;

    struct epoll_event interrupt_event;
    interrupt_event.events  = EPOLLIN;
    interrupt_event.data.fd = interrupt_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, interrupt_fd, &interrupt_event))
        return 0;

    struct epoll_monitor_context_t* ep_ctx =
        calloc(1, sizeof(struct epoll_monitor_context_t));

    ep_ctx->fd           = fd;
    ep_ctx->epoll_fd     = epoll_fd;
    ep_ctx->interrupt_fd = interrupt_fd;

    return (struct monitor_ctx_t*)ep_ctx;
}

void fd_monitor_free(struct monitor_ctx_t* ctx) {
    struct epoll_monitor_context_t* ep_ctx =
        (struct epoll_monitor_context_t*)ctx;

    close(ep_ctx->epoll_fd);
    close(ep_ctx->interrupt_fd);

    free(ep_ctx);
}

int fd_monitor_interrupt(struct monitor_ctx_t* ctx) {
    struct epoll_monitor_context_t* ep_ctx =
        (struct epoll_monitor_context_t*)ctx;

    const uint64_t u = 1;
    if (write(ep_ctx->interrupt_fd, &u, sizeof(uint64_t)) != sizeof(uint64_t))
        return -1;

    return 0;
}

enum poll_result fd_monitor_poll(struct monitor_ctx_t* ctx, int timeout_ms) {
    struct epoll_monitor_context_t* ep_ctx =
        (struct epoll_monitor_context_t*)ctx;

    struct epoll_event events[2];

    int fds_count = epoll_wait(ep_ctx->epoll_fd, events, 2, timeout_ms);
    if (fds_count < 0)
        return error;

    if (fds_count == 0)
        return timeout;

    for (int i = 0; i < fds_count; i++) {
        if (events[i].data.fd == ep_ctx->interrupt_fd) {
            uint64_t buf;
            read(ep_ctx->interrupt_fd, &buf, sizeof(buf));
            return interrupted;
        }
    }

    return data_ready;
}

int fd_monitor_get_fd(struct monitor_ctx_t* ctx) {
    struct epoll_monitor_context_t* ep_ctx =
        (struct epoll_monitor_context_t*)ctx;
    return ep_ctx->fd;
}
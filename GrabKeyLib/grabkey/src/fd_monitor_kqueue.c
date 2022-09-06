#include "fd_monitor.h"
#include <sys/event.h>
#include <sys/pipe.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

struct kqueue_monitor_ctx_t {
    int fd;
    int kqueue_fd;
    int interrupt_pipe[2];
};

int SubscribeOnEvents(int kq, int fd)
{
    // Event we want to monitor
    struct kevent event;

    // Initialize kevent structure
    EV_SET(&event, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

    // Attach event to the kqueue
    if (kevent(kq, &event, 1, NULL, 0, NULL) < 0)
        return -1;

    if (event.flags & EV_ERROR)
        return -1;

    return 0;
}

struct monitor_ctx_t* fd_monitor_create(int fd)
{
    int kq = kqueue();

    if (kq < 0)
        return NULL;

    // Create pipe for kqueue completing
    int interrupt_pipe[2];
    if (pipe(interrupt_pipe) < 0)
        return NULL;

    SubscribeOnEvents(kq, fd);
    SubscribeOnEvents(kq, interrupt_pipe[0]);

    struct kqueue_monitor_ctx_t* kq_ctx = calloc(1, sizeof(struct kqueue_monitor_ctx_t*));
    kq_ctx->fd = fd;
    kq_ctx->kqueue_fd = kq;
    memcpy(kq_ctx->interrupt_pipe, interrupt_pipe, sizeof(interrupt_pipe));

    return (struct monitor_ctx_t*)kq_ctx;
}

void fd_monitor_free(struct monitor_ctx_t* ctx)
{
    struct kqueue_monitor_ctx_t* kq_ctx = (struct kqueue_monitor_ctx_t*)ctx;

    close(kq_ctx->kqueue_fd);
    close(kq_ctx->interrupt_pipe[0]);
    close(kq_ctx->interrupt_pipe[1]);

    free(kq_ctx);
}

int fd_monitor_interrupt(struct monitor_ctx_t* ctx)
{
    struct kqueue_monitor_ctx_t* kq_ctx = (struct kqueue_monitor_ctx_t*)ctx;

    const char s;
    if (write(kq_ctx->interrupt_pipe[1], &s, sizeof(s)) != sizeof(s))
        return -1;

    return 0;
}

static struct timespec* ms_to_timespec(struct timespec* ts, int ms)
{
    if (ms < 0)
        return NULL;
        
    ts->tv_sec = ms / 1000;
    ts->tv_nsec = (ms % 1000) * 1000000;

    return ts;
}

enum poll_result fd_monitor_poll(struct monitor_ctx_t* ctx, int timeout_ms)
{
    struct kqueue_monitor_ctx_t* kq_ctx = (struct kqueue_monitor_ctx_t*)ctx;

    struct timespec timeout_ts;
    struct kevent event; 

    int events_count = kevent(kq_ctx->kqueue_fd, NULL, 0, &event, 1, ms_to_timespec(&timeout_ts, timeout_ms));
    if (events_count < 0)
        return error;
    
    if (events_count == 0)
        return timeout;

    if ((int)event.ident == kq_ctx->interrupt_pipe[0]) {
        uint64_t buf;
        read(kq_ctx->interrupt_pipe[0], &buf, sizeof(buf));
        return interrupted;
    }

    return data_ready;
}

int fd_monitor_get_fd(struct monitor_ctx_t *ctx)
{
    struct kqueue_monitor_ctx_t* kq_ctx = (struct kqueue_monitor_ctx_t*)ctx;
    return kq_ctx->fd;   
}
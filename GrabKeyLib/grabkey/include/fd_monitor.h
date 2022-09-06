#ifndef __FD_MONITOR_H__
#define __FD_MONITOR_H__

enum poll_result {
    error = -1,
    timeout = 0,
    interrupted = 1,
    data_ready = 2
};

struct monitor_ctx_t {};

#ifdef __cplusplus
extern "C"{
#endif 

/**
 * @brief Create monitor for the specified descriptor.
 * 
 * @param fd The descriptor you need to monitor.
 * @return struct monitor_ctx_t* Pointer to context that represents monitor instance.
 */
struct monitor_ctx_t* fd_monitor_create(int fd);

/**
 * @brief Destroy the monitor.
 * 
 * @param ctx Pointer that is returned by fd_monitor_create call.
 */
void fd_monitor_free(struct monitor_ctx_t* ctx);

/**
 * @brief Interrupt fd_monitor_poll waiting.
 * 
 * @param ctx Pointer that is returned by fd_monitor_create call.
 * @return int -1 on the interruption faile and 0 on success.
 */
int fd_monitor_interrupt(struct monitor_ctx_t* ctx);

/**
 * @brief Wait (synchronously) for data to be ready to read from the descriptor.
 * 
 * @param ctx Pointer that is returned by fd_monitor_create call.
 * @param timeout_ms Max time to stuck on the call.
 * @return enum poll_result Describes the result of the polling.
 */
enum poll_result fd_monitor_poll(struct monitor_ctx_t* ctx, int timeout_ms);

/**
 * @brief Get the monitored descriptor.
 * 
 * @param ctx Pointer that is returned by fd_monitor_create call.
 * @return int The monitored descriptor.
 */
int fd_monitor_get_fd(struct monitor_ctx_t* ctx);

#ifdef __cplusplus
}
#endif

#endif // __FD_MONITOR_H__
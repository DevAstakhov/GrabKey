#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#include <termios.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set terminal to send pressed keys to STDIN immediately without buffering.
 * 
 * @return struct termios* Terminal parameters that were before the raw mode is set.
 */
struct termios* terminal_set_raw_mode();

/**
 * @brief Restore the parameters.
 * 
 * @param params Pointer to the parameters to be restored.
 * @return int 0 on success and -1 on fail.
 */
int terminal_restore_mode(const struct termios* params);

/**
 * @brief Free memory pointed by the params.
 * 
 * @param params Pointer returned by terminal_set_raw_mode call.
 */
void terminal_free(struct termios* params);

#ifdef __cplusplus
}
#endif

#endif
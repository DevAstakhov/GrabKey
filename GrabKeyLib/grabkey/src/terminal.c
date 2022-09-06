#include "terminal.h"
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>

struct termios* terminal_set_raw_mode()
{
    struct termios* original_params = calloc(1, sizeof(struct termios));

    if (tcgetattr(STDIN_FILENO, original_params) != 0)
        goto fail_set_raw_mode;

    struct termios new_params;

    memcpy(&new_params, original_params, sizeof(new_params));
    new_params.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);

    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_params) != 0)
        goto fail_set_raw_mode;

    return original_params;

fail_set_raw_mode:
    free(original_params);
    return NULL;
}

int terminal_restore_mode(const struct termios* params)
{
    if (tcsetattr(STDIN_FILENO, TCSANOW, params) < 0)
        return -1;

    return 0;
}

void terminal_free(struct termios* params)
{
    free(params);
    params = NULL;
}

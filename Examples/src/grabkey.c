#include <stdio.h>
#include <unistd.h>
#include "fd_monitor.h"
#include "terminal.h"

int main() {
    // switching terminal to an unbuffered mode
    struct termios* orig_params = terminal_set_raw_mode();

    // now every pressed key immediately goes into stdin
    // let's monitor the stdin buffer to know it get a new data
    struct monitor_ctx_t* mon_ctx = fd_monitor_create(STDIN_FILENO);

    int timeout_sec = 5;
    printf("Timeout is %i sec. Exit key is ESC.\n", timeout_sec);

    int keepGoing = 1;
    do {
        // hold on until something happened
        switch (fd_monitor_poll(mon_ctx, timeout_sec * 1000)) {
            case error:  // somethig went wrong
                printf("Failed to monitor input");
                keepGoing = 0;
                continue;
            case interrupted:  // fd_monitor_interrupt has been called
                printf("Interrupted\n");
                keepGoing = 0;
                continue;
            case timeout:  // the specified time has elapsed
                printf("Timeout\n");
                keepGoing = 0;
                continue;
            case data_ready:  // here we go, there is a data in stdin buffer
            {
                // reading the data
                char buf[1024];
                ssize_t bytes = read(fd_monitor_get_fd(mon_ctx), buf, 1024);

                // detecting if ESC key code is read
                if (bytes == 1 && buf[0] == 27)
                    fd_monitor_interrupt(mon_ctx);

                // printing the sequence
                printf("Sequence: ");
                for (size_t i = 0; i < bytes; ++i)
                    printf("%i ", buf[i]);
                printf("\n");
            } break;
        }
    } while (keepGoing);

    fd_monitor_free(mon_ctx);
    terminal_restore_mode(orig_params);
    terminal_free(orig_params);

    return 0;
}
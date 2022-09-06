#include "raw_mode.h"

namespace terminal {

RawMode::RawMode()
    : the_mode(terminal_set_raw_mode(),
        [](auto mode) { 
            terminal_restore_mode(mode);
            terminal_free(mode);
        })
{}

void RawMode::dispose() { the_mode.reset(); }

}
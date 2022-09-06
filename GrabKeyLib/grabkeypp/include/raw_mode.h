#pragma once

#include "terminal.h"
#include <memory>

namespace terminal {

class RawMode {
    std::shared_ptr<termios> the_mode;
public:
    RawMode();
    void dispose();
};

}
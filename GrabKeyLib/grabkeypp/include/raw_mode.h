#pragma once

#include <memory>

struct termios;

namespace terminal {

class RawMode {
    std::shared_ptr<termios> the_mode;
public:
    RawMode();
    void dispose();
};

}
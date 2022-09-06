#pragma once

#include "detailed_key.h"
#include "IKeyParser.hpp"
#include <cstddef>


namespace keyboard {

class KeyboardKeyParser : public IKeyParser<DetailedKey> {
public:
    Result process_buffer(std::vector<char> buffer) override;
    Result process_timeout() override;
    Result process_interruption() override;
};

Key ParseSequence(const char* data, size_t size);

}

#pragma once

#include <cstddef>
#include "IKeyParser.hpp"
#include "detailed_key.h"

namespace keyboard {

class KeyboardKeyParser : public IKeyParser<DetailedKey> {
public:
    Result process_buffer(std::vector<char> buffer) override;
    Result process_timeout() override;
    Result process_interruption() override;
};

Key ParseSequence(const char* data, size_t size);

}  // namespace keyboard

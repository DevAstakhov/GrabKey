#pragma once

#include <vector>

template <class TResult>
class IKeyParser {
public:
    using Result = TResult;

    virtual Result process_buffer(std::vector<char> buffer) = 0;
    virtual Result process_timeout()                        = 0;
    virtual Result process_interruption()                   = 0;

    virtual ~IKeyParser() = default;
};
#pragma once

#include <functional>
#include <vector>
#include "kbd_keys.h"

namespace keyboard {

struct DetailedKey {
    Key key;
    std::vector<char> buffer;

    DetailedKey(Key key, std::vector<char> buffer = {});
};

}  // namespace keyboard

namespace std {

template <>
struct hash<keyboard::DetailedKey> {
    size_t operator()(const keyboard::DetailedKey& x) const {
        return static_cast<size_t>(x.key);
    }
};

template <>
struct equal_to<keyboard::DetailedKey> {
    constexpr bool operator()(
        const keyboard::DetailedKey& lhs, const keyboard::DetailedKey& rhs
    ) const {
        return lhs.key == rhs.key;
    }
};

}  // namespace std
#include "detailed_key.h"
#include <cstring>

namespace keyboard {

DetailedKey::DetailedKey(Key key, std::vector<char> buffer)
    : key(key), buffer(buffer) {}

}  // namespace keyboard
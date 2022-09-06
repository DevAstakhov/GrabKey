#pragma once

#include <unordered_map>
#include <functional>


namespace keyboard {

template<class TKey, class HashFn, class EqualFn>
struct EventsConfigBase {
    using Handler = std::function<void(const TKey&)>;

    std::unordered_map<TKey, Handler, HashFn, EqualFn> handlers;
    Handler before_handler;
    Handler after_handler;
    Handler default_handler;
};

}
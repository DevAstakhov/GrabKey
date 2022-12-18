#pragma once

#include <chrono>
#include <optional>
#include "events_config.h"
#include "kbd_reader.h"

namespace keyboard {

template <
    class TParser, class ResultHashFn = std::hash<typename TParser::Result>,
    class ResultEqualFn = std::equal_to<typename TParser::Result>>
class EventsProcessorBase {
public:
    using KeyboardReader = KeyboardReaderBase<TParser>;
    using EventsConfig =
        EventsConfigBase<typename TParser::Result, ResultHashFn, ResultEqualFn>;

private:
    KeyboardReader kbd_reader;
    EventsConfig config;

public:
    EventsProcessorBase(
        KeyboardReader kbd_reader = KeyboardReader(), EventsConfig config = {}
    )
        : kbd_reader(kbd_reader), config(config) {}

    typename EventsConfig::Handler get_handler(
        const typename KeyboardReader::Parser::Result& result
    ) {
        auto handler_it = config.handlers.find(result);
        if (handler_it != config.handlers.end())
            return handler_it->second;

        if (config.default_handler)
            return config.default_handler;

        return {};
    }

    void invoke(
        typename EventsConfig::Handler& handler,
        typename KeyboardReader::Parser::Result result
    ) {
        if (config.before_handler)
            config.before_handler(result);

        handler(result);

        if (config.after_handler)
            config.after_handler(result);
    }

    void start(
        std::optional<std::chrono::milliseconds> timeout = std::nullopt
    ) {
        bool working = true;

        do {
            typename KeyboardReader::Status status;

            auto result = kbd_reader.get_key(timeout, status);

            switch (status) {
                case KeyboardReader::Status::TimedOut:
                    continue;
                case KeyboardReader::Status::Interrupted:
                    working = false;
                    break;
                case KeyboardReader::Status::Processed: {
                    auto handler = get_handler(result);
                    if (handler)
                        invoke(handler, result);
                } break;
            }

        } while (working);
    }

    void stop() { kbd_reader.interrupt(); }

    EventsConfig get_config() const { return config; }

    void apply_config(const EventsConfig& config) {
        this->apply_config(EventsConfig(config));
    }

    void apply_config(EventsConfig&& config) { this->config = config; }
};

using EventsProcessor = EventsProcessorBase<KeyboardKeyParser>;

}  // namespace keyboard
#pragma once

#include <unistd.h>
#include <chrono>
#include <deque>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include "fd_reader.h"
#include "kbd_key_parser.h"
#include "raw_mode.h"

namespace keyboard {

template <class TParser>
class KeyboardReaderBase {
public:
    using Parser = TParser;

private:
    terminal::RawMode raw;
    FdReader reader;
    Parser parser;
    size_t read_size;
    std::deque<std::vector<char>> buffers;

public:
    KeyboardReaderBase(Parser parser = Parser(), size_t read_size = 128)
        : reader(STDIN_FILENO), parser(parser), read_size(read_size) {}

    enum class Status {
        Processed   = 0,
        TimedOut    = 1,
        Interrupted = 2
    };

    typename Parser::Result get_key(
        std::optional<std::chrono::milliseconds> timeout = std::nullopt
    ) {
        Status s;
        return get_key(std::move(timeout), s);
    }

    typename Parser::Result get_key(
        std::optional<std::chrono::milliseconds> timeout, Status& out_status
    ) {
        auto process_front_buffer = [this, &out_status]() {
            std::vector<char> buffer = std::move(buffers.front());
            buffers.pop_front();

            const auto key = parser.process_buffer(std::move(buffer));
            out_status     = Status::Processed;

            return key;
        };

        if (!buffers.empty())
            return process_front_buffer();

        switch (reader.poll(std::move(timeout))) {
            case PollResult::DataReady:
                buffers.push_back(reader.read(read_size));
                return process_front_buffer();
            case PollResult::Timeout:
                out_status = Status::TimedOut;
                return parser.process_timeout();
            case PollResult::Interrupted:
                out_status = Status::Interrupted;
                return parser.process_interruption();
        }

        throw std::runtime_error("Unreachable");
    }

    void interrupt() const { reader.interrupt(); }
};

using KeyboardReader = KeyboardReaderBase<KeyboardKeyParser>;

}  // namespace keyboard
#pragma once

#include "fd_reader.h"
#include "kbd_key_parser.h"
#include <deque>
#include <type_traits>
#include <unistd.h>


namespace keyboard {

template<class TParser>
class KeyboardReaderBase {
public:
    using Parser = TParser;

private:
    FdReader reader;
    Parser parser;
    size_t read_size;
    std::deque<std::vector<char>> buffers;

public:
    KeyboardReaderBase(Parser parser = Parser(), size_t read_size = 128)
        : reader(STDIN_FILENO)
        , parser(parser)
        , read_size(read_size)
    {}

    enum class Status {
        Processed = 0,
        TimedOut = 1,
        Interrupted = 2
    };

    typename Parser::Result get_key(int timeout_ms) {
        Status s;
        return get_key(timeout_ms, s);
    }

    typename Parser::Result get_key(int timeout_ms, Status& out_status) {        
        auto process_front_buffer = [this, &out_status]() {
            std::vector<char> buffer = std::move(buffers.front());
            buffers.pop_front();
            const auto key = parser.process_buffer(std::move(buffer));
            out_status = Status::Processed;
            return key;
        };

        if (!buffers.empty())
            return process_front_buffer();

        switch (reader.poll(timeout_ms)) {
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
    }

    void interrupt() const { reader.interrupt(); }
};

using KeyboardReader = KeyboardReaderBase<KeyboardKeyParser>;

}
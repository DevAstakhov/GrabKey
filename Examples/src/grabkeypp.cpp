#include "IKeyParser.hpp"
#include "detailed_key.h"
#include "events_config.h"
#include "events_processor.h"
#include "kbd_key_parser.h"
#include "kbd_keys.h"
#include "kbd_reader.h"
#include <cstddef>
#include <exception>
#include <iostream>
#include <unordered_map>

void print_usage(const char* bin_name);
void use_reader_basic();
void use_processor_basic();
void use_reader_advanced();
void use_processor_advanced();

std::unordered_map<
    std::string, 
    std::unordered_map<
        std::string, 
        std::function<void()>
    >
> functions = {
    { "basic", { 
        { "reader", use_reader_basic },
        { "processor", use_processor_basic } 
    }},
    { "advanced", {
            { "reader", use_reader_advanced },
            { "processor", use_processor_advanced }
    }}
};

int main(int argc, char* argv[]) {
    try {

        if (argc == 3)
            functions.at(argv[1]).at(argv[2])();
        else
            print_usage(argv[0]);

    } catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void print_usage(const char* bin_name) {
    std::cout << "Usage: " << bin_name << " <basic | advanced> <reader | processor>" << std::endl;
}

// Keyboard reader is designed to read and parse keyboard sequences
void use_reader_basic() {
    // Create a KeyboardReader
    keyboard::KeyboardReader reader;

    // Some variables with initial states
    bool keep_going = true;
    int timeout_sec = 5;

    do {
        // Trying to get a keyboard key
        keyboard::KeyboardReader::Status status{};
        auto [key, sequense] = reader.get_key(timeout_sec * 1000, status);

        switch (status) {
            // The specified time has elapsed
            case keyboard::KeyboardReader::Status::TimedOut:
                std::cout << "Timeout" << std::endl;
                keep_going = false;
                reader.interrupt();
                continue;
            // keyboard::KeyboardReader::interrupt() was called
            case keyboard::KeyboardReader::Status::Interrupted:
                std::cout << "Interrupted" << std::endl;
                keep_going = false;
                reader.interrupt();
                continue;
            // A key has been read
            case keyboard::KeyboardReader::Status::Processed:
                // Reacting on the key
                switch (key) {
                    case keyboard::Key::UP:
                    case keyboard::Key::DOWN:
                    case keyboard::Key::RIGHT:
                    case keyboard::Key::LEFT:
                    case keyboard::Key::ENTER:
                        // printing just raw data that was read from stdin
                        std::cout << sequense.data() << std::flush;
                        break;
                    case keyboard::Key::ESC:
                        // Interruption status will be returned on next iteration
                        reader.interrupt();
                        break;
                    default:
                        if (sequense.size() == 1) // this is most likely a printable character
                            // just print it
                            std::cout << sequense.data() << std::flush;
                        break;
                }

                break;
            default:
                break;
        }

    } while(keep_going);
}

// EventsProcessor is designed to encapsulate reading and parsing logic
// and concentrate on the target funtions to be invoked.
void use_processor_basic() {
    // Create an EventsProcessor
    keyboard::EventsProcessor processor;
    
    // Get copy of the existing configuration
    auto config = processor.get_config();

    // Configure reactions on your keys
    config.handlers[keyboard::Key::ESC] = [&processor](auto) { processor.stop(); };
    config.default_handler = [](auto key) { std::cout << key.buffer.data(); };
    config.after_handler = [](auto) { std::cout << std::flush; };
    
    // Set the configuration back to the processor
    processor.apply_config(config);

    // Run the processor's loop
    processor.start();
}

// The example shows how to implement your custom parser and result type
void use_reader_advanced() {
    // Let's imagine we are creating a console game
    
    // Create a type you want to KeyboardReader returns
    struct MyResult {
        enum Key {
            ESC = -1,
            NONE = 0,
            KEY_W = int('w'),
            KEY_S = int('s'),
            KEY_A = int('a'),
            KEY_D = int('d')
        } key;

        bool is_shift_pressed = false;
    };

    // Implement an IKeyParser which parses ansi sequence and codes 
    // and returns the result object create above
    class MyParser : public IKeyParser<MyResult> {
    public:
        // This is the main function that parse data from keyboard 
        // and turn it into your comfortable result type
        MyResult process_buffer(std::vector<char> buffer) override {
            if (buffer.size() != 1)
                return { MyResult::NONE, false };
            
            if (buffer[0] == 'w') return { MyResult::KEY_W, false };
            if (buffer[0] == 'W') return { MyResult::KEY_W, true };
            if (buffer[0] == 's') return { MyResult::KEY_S, false };
            if (buffer[0] == 'S') return { MyResult::KEY_S, true };
            if (buffer[0] == 'a') return { MyResult::KEY_A, false };
            if (buffer[0] == 'A') return { MyResult::KEY_A, true };
            if (buffer[0] == 'd') return { MyResult::KEY_D, false };
            if (buffer[0] == 'D') return { MyResult::KEY_D, true };
            if (buffer[0] == char(27)) return { MyResult::ESC, false };

            return { MyResult::NONE, false };
        }

        // You may want to use this timeout signal in your own purpose.
        // This method is called if the desired time 
        // that is passed into get_key method has elapsed.
        Result process_timeout() override {
            return { MyResult::NONE, false };
        }

        // This method is called when reader.interrupt() method is called
        Result process_interruption() override {
            return { MyResult::ESC, false };
        }
    };

    // It's time to finally create the reader and handle results
    keyboard::KeyboardReaderBase<MyParser> reader;
    
    bool working = true;
    while (working) {
        // Waiting for a key to be read
        const MyResult result = reader.get_key(-1);

        // Clearing line
        std::cout << "\r" << "\033[K" << "\r" << std::flush;

        // Reacting on the key
        switch (result.key) {
            case MyResult::KEY_W: std::cout << "Going UP"; break;
            case MyResult::KEY_S: std::cout << "Going DOWN"; break;
            case MyResult::KEY_A: std::cout << "Turning LEFT"; break;
            case MyResult::KEY_D: std::cout << "Turning RIGHT"; break;
            case MyResult::ESC: working = false;
            default: continue;
        }

        // Additional reaction
        if (result.is_shift_pressed)
            std::cout << " fast";
        else
            std::cout << " slow";

        // Flushing iostream buffer
        std::cout << std::flush;
    }
}

// This example shows how to use keyboard events processor 
// with custom parser and result type
void use_processor_advanced() {
    // let's imagine we are creating a console game

    // Create a type you want to processor pass into handlers
    struct MyResult {
        enum Key {
            ESC = -1,
            NONE = 0,
            KEY_W = int('w'),
            KEY_S = int('s'),
            KEY_A = int('a'),
            KEY_D = int('d')
        } key;

        bool is_shift_pressed = false;

        MyResult(Key key, bool shift = false) :
            key(key), is_shift_pressed(shift) {}

        // this operator makes able to get hash of the object with std::hash<int> function
        operator int() const {
            return int(key);
        }

        // and this is a simple comparation method
        bool operator == (const MyResult& other) const {
            return this->key == other.key;
        }

        // note that the operators don't use is_shift_pressed field
        // hence 'W' and 'w' (and other keys) are going to be treated as identical objects
    };

    // create a parser to parse ansi sequence and codes
    class MyParser : public IKeyParser<MyResult> {
    public:
        // this is the main function that parse data from keyboard 
        // and turn it into your comfortable result type
        MyResult process_buffer(std::vector<char> buffer) override {
            if (buffer.size() != 1)
                return { MyResult::NONE, false };
            
            if (buffer[0] == 'w') return { MyResult::KEY_W, false };
            if (buffer[0] == 'W') return { MyResult::KEY_W, true };
            if (buffer[0] == 's') return { MyResult::KEY_S, false };
            if (buffer[0] == 'S') return { MyResult::KEY_S, true };
            if (buffer[0] == 'a') return { MyResult::KEY_A, false };
            if (buffer[0] == 'A') return { MyResult::KEY_A, true };
            if (buffer[0] == 'd') return { MyResult::KEY_D, false };
            if (buffer[0] == 'D') return { MyResult::KEY_D, true };
            if (buffer[0] == char(27)) return { MyResult::ESC, false };

            return { MyResult::NONE, false };
        }

        // You may want to use this timeout signal in your own purpose.
        // This method is called if the desired time 
        // that is passed into get_key method has elapsed.
        Result process_timeout() override {
            return { MyResult::NONE, false };
        }

        // This methid is called when reader.interrupt() method is called
        Result process_interruption() override {
            return { MyResult::ESC, false };
        }
    };

    // finally creating the processor
    keyboard::EventsProcessorBase<MyParser, std::hash<int>> processor;
    // take the existing configuration (it's actually empty)
    auto config = processor.get_config();
    
    // setup handlers
    // the before_handler is going to erase line before any other handler is invoked
    config.before_handler = [](MyResult) { std::cout << "\r" << "\033[K" << "\r" << std::flush; };
    
    // these handlers are the desired reactions on the keys
    config.handlers[MyResult::KEY_W] = [](MyResult key) { std::cout << "Going UP "; };
    config.handlers[MyResult::KEY_S] = [](MyResult key) { std::cout << "Going DOWN "; };
    config.handlers[MyResult::KEY_A] = [](MyResult key) { std::cout << "Turning LEFT "; };
    config.handlers[MyResult::KEY_D] = [](MyResult key) { std::cout << "Turning RIGHT "; };

    // do not forget to add handler which stops the processor
    config.handlers[MyResult::ESC] = [&processor](MyResult) { processor.stop(); };
    
    // the after_handler adds some common information and flushes the text
    config.after_handler = [](MyResult key) { 
        std::cout << (key.key != MyResult::ESC ? (key.is_shift_pressed ? "fast" : "slow") : "") << std::flush; 
    };

    // apply the config to the processor
    processor.apply_config(config);

    // start the processor loop
    processor.start();
}
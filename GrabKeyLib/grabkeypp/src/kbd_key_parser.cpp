#include <vector>
#include <array>

#include "kbd_key_parser.h"


namespace keyboard {

static constexpr std::array<char, 1> seq_esc { 27 };
static constexpr std::array<char, 3> seq_up   { 27, 91, 65 };
static constexpr std::array<char, 3> seq_down { 27, 91, 66 };
static constexpr std::array<char, 3> seq_right { 27, 91, 67 };
static constexpr std::array<char, 3> seq_left { 27, 91, 68 };
static constexpr std::array<char, 3> seq_shift_tab { 27, 91, 90 };
static constexpr std::array<char, 1> seq_enter { '\n' };
static constexpr std::array<char, 1> seq_space { ' ' };
static constexpr std::array<char, 1> seq_tab { '\t' };
static constexpr std::array<char, 3> seq_f1 { 27, 79, 80 };
static constexpr std::array<char, 3> seq_f2 { 27, 79, 81 };
static constexpr std::array<char, 3> seq_f3 { 27, 79, 82 };
static constexpr std::array<char, 3> seq_f4 { 27, 79, 83 };
static constexpr std::array<char, 5> seq_f5 { 27, 91, 49, 53, 126 };
static constexpr std::array<char, 5> seq_f6 { 27, 91, 49, 55, 126 };
static constexpr std::array<char, 5> seq_f7 { 27, 91, 49, 56, 126 };
static constexpr std::array<char, 5> seq_f8 { 27, 91, 49, 57, 126 };
static constexpr std::array<char, 5> seq_f9 { 27, 91, 50, 48, 126 };
static constexpr std::array<char, 5> seq_f10 { 27, 91, 50, 49, 126 };
static constexpr std::array<char, 5> seq_f11 { 27, 91, 50, 51, 126 };
static constexpr std::array<char, 5> seq_f12 { 27, 91, 50, 52, 126 };


Key ParseSequence(const char* data, size_t size) {
    
    auto equal = [data, size](auto seq) {
        return size == seq.size() && std::equal(seq.cbegin(), seq.cend(), data);
    };

    if (equal(seq_esc)) return keyboard::Key::ESC;
    if (equal(seq_up)) return keyboard::Key::UP;
    if (equal(seq_down)) return keyboard::Key::DOWN;
    if (equal(seq_right)) return keyboard::Key::RIGHT;
    if (equal(seq_left)) return keyboard::Key::LEFT;
    if (equal(seq_enter)) return keyboard::Key::ENTER;
    if (equal(seq_space)) return keyboard::Key::SPACE;
    if (equal(seq_tab)) return keyboard::Key::TAB;
    if (equal(seq_shift_tab)) return keyboard::Key::SHIFT_TAB;

    if (equal(seq_f1)) return keyboard::Key::F1;
    if (equal(seq_f2)) return keyboard::Key::F2;
    if (equal(seq_f3)) return keyboard::Key::F3;
    if (equal(seq_f4)) return keyboard::Key::F4;
    if (equal(seq_f5)) return keyboard::Key::F5;
    if (equal(seq_f6)) return keyboard::Key::F6;
    if (equal(seq_f7)) return keyboard::Key::F7;
    if (equal(seq_f8)) return keyboard::Key::F8;
    if (equal(seq_f9)) return keyboard::Key::F9;
    if (equal(seq_f10)) return keyboard::Key::F10;
    if (equal(seq_f11)) return keyboard::Key::F11;
    if (equal(seq_f12)) return keyboard::Key::F12;
    
    return keyboard::Key::OTHER;
}

KeyboardKeyParser::Result KeyboardKeyParser::process_buffer(std::vector<char> buffer) {
    return { ParseSequence(buffer.data(), buffer.size()), std::move(buffer) };
}

KeyboardKeyParser::Result KeyboardKeyParser::process_timeout() {
    return { Key::NONE, {} };
}

KeyboardKeyParser::Result KeyboardKeyParser::process_interruption() {
    return { Key::NONE, {} };
}

}
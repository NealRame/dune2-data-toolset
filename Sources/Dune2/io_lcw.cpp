#include "io.hpp"

namespace nr::dune2::io {

std::vector<uint8_t>
readLCWData(std::istream &input, size_t deflated_size, size_t inflated_size) {
    std::vector<uint8_t> dst;

    dst.reserve(inflated_size);

    const auto read_byte = [&]() { return io::readLEInteger<1, uint8_t>(input); };
    const auto read_word = [&]() { return io::readLEInteger<2, uint16_t>(input); };
    const auto copy_block = [&](auto count, auto pos, bool relative) {
        std::copy_n(
            relative
                ? dst.end() - pos
                : dst.begin() + pos,
            count,
            std::back_inserter(dst)
        );
    };

    // Ignore first byte if it is the relative mode flag
    const auto relative = input.peek() == 0;
    if (relative) input.ignore(1);

    // LCW data should end with a 0x80 byte.
    // We also have a given amount of bytes we may read before ending the 
    // deflate process.
    // Well-formed LCW data should match flag 0x80 with the maximum number of
    // bytes of data to read reached.
    const auto end_pos = size_t(input.tellg()) + deflated_size;

    for (uint8_t cmd; (cmd = read_byte()) != 0x80 && input.tellg() < end_pos;) {
        if ((cmd & 0xc0) == 0x80) {
            // command 1: short copy
            // 0b10cccccc
            const auto count = cmd & 0x3f;
            const auto pos   = dst.size();
            dst.resize(dst.size() + count);
            input.read(reinterpret_cast<char *>(dst.data()) + pos, count);
        } else if ((cmd & 0x80) == 0) {
            // command 2: existing block relative copy
            // 0b0cccpppp p
            const auto count = ((cmd & 0x70)>>4) + 3;
            const auto pos   = ((cmd & 0x0f)<<8) | read_byte();
            if (pos == 1) {
                dst.insert(dst.end(), count, dst.back());
            } else {
                copy_block(count, pos, true);
            }
        } else if (cmd == 0xfe) {
            // command 4: repeat value
            // 0b11111110 c c v
            const auto count = read_word();
            const auto value = read_byte();
            dst.insert(dst.end(), count, value);
        } else if (cmd == 0xff) {
            // command 5: existing block long copy
            // 0b11111111 c c p p
            const auto count = read_word();
            const auto pos   = read_word();
            copy_block(count, pos, relative);
        } else {
            // command 3: existing block medium-length copy
            // 0b11cccccc p p
            const auto count = (cmd & 0x3f) + 3;
            const auto pos   = read_word();
            copy_block(count, pos, relative);
        }
    }

    return dst;
}
}

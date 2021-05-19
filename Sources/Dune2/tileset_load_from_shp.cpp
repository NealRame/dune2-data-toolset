#include "tileset.hpp"
#include "io.hpp"

#include <stdexcept>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
namespace nr::dune2 {

namespace {

enum class SHPVersion {
    v100 = 100,
    v107 = 107,
};

SHPVersion
shp_read_version(std::ifstream &input) {
    io::IPosOffsetGuard _(input);
    input.seekg(4);
    return nr::dune2::io::readLEInteger<2>(input) != 0
        ? SHPVersion::v100
        : SHPVersion::v107;
}

template<SHPVersion V>
std::istream::pos_type
shp_read_frame_offset(std::istream &input) {
    if constexpr(V == SHPVersion::v100) {
        return io::readLEInteger<2, std::uint16_t>(input);
    } else {
        return io::readLEInteger<4, std::uint32_t>(input) + 2;
    }
}

std::vector<std::istream::pos_type>
shp_read_frame_offsets(std::istream &input, SHPVersion version) {
    std::vector<std::istream::pos_type> offsets;
    const auto frame_count = io::readLEInteger<2, unsigned int>(input);
    std::generate_n(
        std::back_inserter(offsets),
        frame_count,
        [&]() {
            return version == SHPVersion::v100
                ? shp_read_frame_offset<SHPVersion::v100>(input)
                : shp_read_frame_offset<SHPVersion::v107>(input);
        }
    );
    return offsets;
}

void
shp_read_data(std::istream &input, size_t count, std::vector<uint8_t> &dst) {
    const auto pos = dst.size();
    dst.resize(dst.size() + count);
    input.read(reinterpret_cast<char *>(dst.data()) + pos, count);
}

void
shp_lcw_deflate(std::istream &input, size_t size, std::vector<uint8_t> &dst) {
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
    const auto end_pos = size_t(input.tellg()) + size;

    for (uint8_t cmd; (cmd = read_byte()) != 0x80 && input.tellg() < end_pos;) {
        if ((cmd & 0xc0) == 0x80) {
            // command 1: short copy
            // 0b10cccccc
            const auto count = cmd & 0x3f;
            shp_read_data(input, count, dst);
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
}

Tileset::Tile
shp_read_tile(std::istream &input, std::istream::pos_type pos) {
    input.seekg(pos);

    std::bitset<16> frame_flags(io::readLEInteger<2>(input));

    static const auto HasRemapTable = 0u;
    static const auto NoLCW = 1u;
    static const auto CustomSizeRemap = 2u;

    input.ignore(1);

    auto width = io::readLEInteger<2, size_t>(input);
    auto height = io::readLEInteger<1, size_t>(input);

    std::string data;
    std::string data_remap_table;

    const auto frame_size = io::readLEInteger<2, size_t>(input);
    const auto rle_data_size = io::readLEInteger<2, size_t>(input);

    if (frame_flags[HasRemapTable]) {                        // HasRemapTable is set
        const auto remap_size = frame_flags[CustomSizeRemap] // CustomSizeRemap is set
            ? io::readLEInteger<1, size_t>(input)
            : 16;
        
        data_remap_table = io::readString(input, remap_size);
    }

    const auto frame_data_size = frame_size - (input.tellg() - pos);

    std::vector<uint8_t> rle_data;
    rle_data.reserve(rle_data_size);
    if (frame_flags[1]) {
        shp_read_data(input, frame_data_size, rle_data);
    } else {
        shp_lcw_deflate(input, frame_data_size, rle_data);
    }

    auto it = rle_data.begin();
    while (it != rle_data.end()) {
        const auto n = it - rle_data.begin();
        const auto value = *it++;
        const auto count = (value == 0 ? *it++ : 1);
        data.insert(data.end(), count, value);
    }

    return Tileset::Tile(width, height, std::move(data), std::move(data_remap_table));
}

} // namespace

void
Tileset::loadFromSHP(const fs::path &shp_path) {
    std::ifstream input;

    input.open(shp_path, std::ios::binary);
    input.exceptions(std::ios::failbit);

    const auto version = shp_read_version(input);
    const auto offsets = shp_read_frame_offsets(input, version);

    std::transform(
        offsets.begin(),
        offsets.end(),
        std::back_inserter(tiles_),
        [&](const auto pos) {
            return shp_read_tile(input, pos);
        }
    );
}

} // namespace nr::dune2

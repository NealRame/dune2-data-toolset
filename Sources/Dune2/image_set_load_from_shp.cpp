#include "image_set.hpp"
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

ImageSet::Image
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
    if (frame_flags[1]) {
        rle_data = io::readData(input, frame_data_size);
    } else {
        rle_data = io::readLCWData(input, frame_data_size, rle_data_size);
    }

    auto it = rle_data.begin();
    while (it != rle_data.end()) {
        const auto n = it - rle_data.begin();
        const auto value = *it++;
        const auto count = (value == 0 ? *it++ : 1);
        data.insert(data.end(), count, value);
    }

    return ImageSet::Image(width, height, std::move(data), std::move(data_remap_table));
}

} // namespace

void
ImageSet::loadFromSHP(const fs::path &shp_path) {
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

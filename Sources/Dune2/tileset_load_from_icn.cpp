#include "tileset.hpp"
#include "io.hpp"

#include <stdexcept>
#include <fstream>
#include <iostream>

namespace nr::dune2 {

namespace {


struct ICNInfo {
    /// #### attribute `width`
    /// The pixels width of a Tile.
    size_t width;

    /// #### attribute `height`
    /// The pixels height of a Tile.
    size_t height;

    /// #### attribute `bitPerPixels`
    /// The number of bits used to store one pixel.
    size_t bitPerPixels;

    /// #### method `Tile.getTileSize`
    /// ##### Return
    /// `size_t` - the number of bytes used to store a tile.
    size_t getTileSize() const
    { return (width*height*bitPerPixels)/8; }

    /// #### method `Tile.getPaletteSize`
    /// ##### Return
    /// `size_t` - the number of colors used in a tile.
    size_t getPaletteSize() const
    { return 1<<bitPerPixels; }
};

using SSet = std::vector<std::string>;
using RPal = std::vector<std::string>;
using RTbl = std::vector<uint8_t>;

ICNInfo
read_sinf_chunk(std::ifstream &input) {
    using namespace std::literals;

    io::check(input, []{ return "SINF"sv; });

    const auto sinf_chunk_size = io::readBEInteger<4>(input);

    const auto width  = io::readLEInteger<1>(input);
    const auto height = io::readLEInteger<1>(input);
    const auto shift  = io::readLEInteger<1>(input);

    const auto bit_per_pixel = io::readLEInteger<1>(input);

    return ICNInfo{
        width  << shift,
        height << shift,
        bit_per_pixel
    };
}

SSet
read_sset_chunk(std::ifstream &input, const ICNInfo &info) {
    using namespace std::literals;

    io::check(input, []{ return "SSET"sv; });

    const auto sset_chunk_size = io::readBEInteger<4>(input);
    const auto tile_size = info.getTileSize();
    const auto tile_count = (sset_chunk_size - 4)/tile_size;

    // We ignore the four next bytes.
    // They must contains 00 00 AB CD 00 00 00 00.
    // ABCD being little endian representation of sset_chunk_size - 4.
    input.ignore(8);

    SSet sset;
    std::generate_n(
        std::back_inserter(sset),
        tile_count,
        [&] { return io::readString(input, tile_size); }
    );
    return sset;
}

RPal
read_rpal_chunk(std::ifstream &input, const ICNInfo &info) {
    using namespace std::literals;

    io::check(input, []{ return "RPAL"sv; });

    const auto rpal_chunk_size = io::readBEInteger<4>(input);
    const auto pal_size = info.getPaletteSize();
    const auto pal_count = rpal_chunk_size/pal_size;

    RPal rpal;
    std::generate_n(
        std::back_inserter(rpal),
        pal_count,
        [&] { return io::readString(input, pal_size); }
    );
    return rpal;
}

RTbl
read_rtbl_chunk(std::ifstream &input, const ICNInfo &info) {
    using namespace std::literals;

    io::check(input, []{ return "RTBL"sv; });

    const auto rtbl_chunk_size = io::readBEInteger<4>(input);
    const auto rtbl = io::readData<uint8_t>(input, rtbl_chunk_size);

    return rtbl;
}
} // namespace

void
Tileset::loadFromICN(const std::string &icn_path) {
    using namespace std::literals;
    std::ifstream input;

    input.open(icn_path, std::ios::binary);
    input.exceptions(std::ios::failbit);

    // First read IFF chunk group ID (wich must be FORM)
    io::check(input, []{ return "FORM"sv;});

    const auto chunk_group_size = io::readBEInteger<4>(input);

    // Check group type is ICON
    io::check(input, []{ return "ICON"sv;});

    // Keep read order below
    const auto info = read_sinf_chunk(input);
    const auto sset = read_sset_chunk(input, info);
    const auto rpal = read_rpal_chunk(input, info);
    const auto rtbl = read_rtbl_chunk(input, info);

    if (sset.size() != rtbl.size()) {
        throw std::invalid_argument("corrupted file");
    }

    std::transform(
        sset.begin(), sset.end(),
        rtbl.begin(),
        std::back_inserter(tiles_),
        [&](auto &raw_data, auto rpal_index) {
            const auto bpp = info.bitPerPixels;
            std::string data;

            for (auto &&b: raw_data) {
                for (auto i = 8/bpp; i > 0; --i) {
                    const auto p = (b >> (i - 1)*bpp) & ((1 << bpp) - 1);
                    data.push_back(rpal[rpal_index][p]);
                }
            }

            return Tile(info.width, info.height, std::move(data));
        }
    );
}

} // namespace nr::dune2

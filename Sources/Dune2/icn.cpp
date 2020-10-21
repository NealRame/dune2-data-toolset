#include "icn.hpp"
#include "io.hpp"

#include <fstream>
#include <iostream>

namespace nr::dune2 {

namespace {

using SSet = std::vector<std::vector<uint8_t>>;
using RPal = std::vector<std::vector<uint8_t>>;
using RTbl = std::vector<uint8_t>;

ICN::Info
read_sinf_chunk(std::fstream &input) {
    using namespace std::literals;

    io::check(input, []{ return "SINF"sv; });

    const auto sinf_chunk_size = io::readBEInteger<4>(input);

    const auto width  = io::readLEInteger<1>(input);
    const auto height = io::readLEInteger<1>(input);
    const auto shift  = io::readLEInteger<1>(input);

    const auto bit_per_pixel = io::readLEInteger<1>(input);

    return ICN::Info{
        width  << shift,
        height << shift,
        bit_per_pixel
    };
}

SSet
read_sset_chunk(std::fstream &input, const ICN::Info &info) {
    using namespace std::literals;

    io::check(input, []{ return "SSET"sv; });

    const auto sset_chunk_size = io::readBEInteger<4>(input);
    const auto tile_size = info.tileSize();
    const auto tile_count = (sset_chunk_size - 4)/tile_size;

    // We ignore the four next bytes.
    // They must contains 00 00 AB CD 00 00 00 00.
    // ABCD being little endian representation of sset_chunk_size - 4.
    input.ignore(8);

    SSet sset;
    std::generate_n(
        std::back_inserter(sset),
        tile_count,
        [&] { return io::readData<uint8_t>(input, tile_size); }
    );
    return sset;
}

RPal
read_rpal_chunk(std::fstream &input, const ICN::Info &info) {
    using namespace std::literals;

    io::check(input, []{ return "RPAL"sv; });

    const auto rpal_chunk_size = io::readBEInteger<4>(input);
    const auto pal_size = info.paletteSize();
    const auto pal_count = rpal_chunk_size/pal_size;

    RPal rpal;
    std::generate_n(
        std::back_inserter(rpal),
        pal_count,
        [&] { return io::readData<uint8_t>(input, pal_size); }
    );
    return rpal;
}

RTbl
read_rtbl_chunk(std::fstream &input, const ICN::Info &info) {
    using namespace std::literals;

    io::check(input, []{ return "RTBL"sv; });

    const auto rtbl_chunk_size = io::readBEInteger<4>(input);
    const auto rtbl = io::readData<uint8_t>(input, rtbl_chunk_size);

    return rtbl;
}

} // namespace


std::optional<ICN>
ICN::load(const std::string &filepath) {
    using namespace std::literals;
    std::fstream input(filepath, std::ios::binary|std::ios::in);

    auto icn = std::make_optional<ICN>();

    try {
        // First read IFF chunk group ID (wich must be FORM)
        io::check(input, []{ return "FORM"sv;});

        const auto chunk_group_size = io::readBEInteger<4>(input);

        // Check group type is ICON
        io::check(input, []{ return "ICON"sv;});

        auto sinf = read_sinf_chunk(input);
        auto sset = read_sset_chunk(input, sinf);
        auto rpal = read_rpal_chunk(input, sinf);
        auto rtbl = read_rtbl_chunk(input, sinf);

        assert(sset.size() == rtbl.size());

        icn->info_ = sinf;
        std::transform(
            sset.begin(), sset.end(),
            rtbl.begin(),
            std::back_inserter(icn->tiles_),
            [&] (auto &tile_data, auto rtbl_index) {
                assert(rtbl_index < rpal.size());
                return ICN::Tile{
                    sinf,
                    rpal[rtbl_index],
                    tile_data,
                };
            }
        );

    } catch (...) {
        return std::nullopt;
    }

    return icn;
}

ICN::const_iterator
ICN::begin() const {
    return tiles_.begin();
}

ICN::const_iterator
ICN::end() const {
    return tiles_.end();
}

ICN::const_iterator
ICN::cbegin() const {
    return begin();
}

ICN::const_iterator
ICN::cend() const {
    return end();
}

uint8_t
ICN::Tile::operator[](std::size_t index) const {
    const auto k = (index*info.bitPerPixels)/8;
    const auto s = info.bitPerPixels - (index*info.bitPerPixels)%8;
    const auto p = (data[k] >> s) & ((1 << info.bitPerPixels) - 1);
    return paletteOffsets[p];
}

} // namespace nr::dune2

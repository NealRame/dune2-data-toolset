#include "tileset.hpp"
#include "io.hpp"

#include <stdexcept>
#include <fstream>
#include <iostream>

namespace nr::dune2 {

namespace {

using SSet = std::vector<std::vector<uint8_t>>;
using RPal = std::vector<std::vector<uint8_t>>;
using RTbl = std::vector<uint8_t>;

Tileset::Tile::Info
read_sinf_chunk(std::fstream &input) {
    using namespace std::literals;

    io::check(input, []{ return "SINF"sv; });

    const auto sinf_chunk_size = io::readBEInteger<4>(input);

    const auto width  = io::readLEInteger<1>(input);
    const auto height = io::readLEInteger<1>(input);
    const auto shift  = io::readLEInteger<1>(input);

    const auto bit_per_pixel = io::readLEInteger<1>(input);

    return Tileset::Tile::Info{
        width  << shift,
        height << shift,
        bit_per_pixel
    };
}

SSet
read_sset_chunk(std::fstream &input, const Tileset::Tile::Info &info) {
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
        [&] { return io::readData<uint8_t>(input, tile_size); }
    );
    return sset;
}

RPal
read_rpal_chunk(std::fstream &input, const Tileset::Tile::Info &info) {
    using namespace std::literals;

    io::check(input, []{ return "RPAL"sv; });

    const auto rpal_chunk_size = io::readBEInteger<4>(input);
    const auto pal_size = info.getPaletteSize();
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
read_rtbl_chunk(std::fstream &input, const Tileset::Tile::Info &info) {
    using namespace std::literals;

    io::check(input, []{ return "RTBL"sv; });

    const auto rtbl_chunk_size = io::readBEInteger<4>(input);
    const auto rtbl = io::readData<uint8_t>(input, rtbl_chunk_size);

    return rtbl;
}
} // namespace

Tileset::Tile::Tile(
    const Info &info,
    const std::vector<uint8_t> &data,
    const std::vector<uint8_t> &paletteIndexes)
    : info_{info}
    , data_{data}
    , paletteIndexes_{paletteIndexes} {
}

std::size_t
Tileset::Tile::getWidth() const {
    return info_.width;
}

std::size_t
Tileset::Tile::getHeight() const {
    return info_.width;
}

std::size_t
Tileset::Tile::getPixel(std::size_t x, std::size_t y) const {
    const auto index = y*getWidth() + x;

    const auto k = (index*info_.bitPerPixels)/8;
    const auto s = info_.bitPerPixels - (index*info_.bitPerPixels)%8;
    const auto p = (data_[k] >> s) & ((1 << info_.bitPerPixels) - 1);

    // return palette_[paletteIndexes_[p]];
    return paletteIndexes_[p];
}

std::optional<Tileset>
Tileset::load(const std::string &icn_path) {
    using namespace std::literals;
    std::fstream icn_input(icn_path, std::ios::binary|std::ios::in);

    auto icn = std::make_optional<Tileset>();

    try {
        // First read IFF chunk group ID (wich must be FORM)
        io::check(icn_input, []{ return "FORM"sv;});

        const auto chunk_group_size = io::readBEInteger<4>(icn_input);

        // Check group type is ICON
        io::check(icn_input, []{ return "ICON"sv;});

        auto info = read_sinf_chunk(icn_input);

        icn->tileInfo_ = info;
        
        // Keep read order below
        icn->tilesDataTable_ = read_sset_chunk(icn_input, info);
        icn->tilesPaletteIndexes_ = read_rpal_chunk(icn_input, info);
        icn->tilesPaletteIndexesTable_ = read_rtbl_chunk(icn_input, info);

        if (icn->tilesDataTable_.size() != icn->tilesPaletteIndexesTable_.size()) {
            throw std::invalid_argument("corrupted file");
        }
    } catch (...) {
        return std::nullopt;
    }

    return icn;
}

std::size_t
Tileset::getTileCount() const {
    return tilesDataTable_.size();
}

Tileset::Tile
Tileset::getTile(std::size_t tile_index) const {
    assert(tilesPaletteIndexesTable_.size() == getTileCount());
    return Tile(
        tileInfo_,
        tilesDataTable_[tile_index],
        tilesPaletteIndexes_[tilesPaletteIndexesTable_[tile_index]]
    );
}

Tileset::TileIterator
Tileset::begin() const {
    using namespace std::placeholders;
    return TileIterator(std::bind(&Tileset::getTile, this, _1), 0);
}

Tileset::TileIterator 
Tileset::end() const {
    return TileIterator([](auto tile_index) -> Tile {
        throw std::out_of_range("tile index out of range");
    }, getTileCount());
}

} // namespace nr::dune2

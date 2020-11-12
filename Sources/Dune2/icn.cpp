#include "icn.hpp"
#include "io.hpp"

#include <stdexcept>
#include <fstream>
#include <iostream>

namespace nr::dune2 {

namespace {

using IMap = std::vector<std::vector<unsigned int>>;

IMap
read_icons_map(std::fstream &input) {
    std::vector<unsigned int> tile_indexes;
    while (!input.eof()) {
        tile_indexes.emplace_back(io::readLEInteger<2>(input));
    }

    const auto count = tile_indexes[0];

    IMap icons_map;
    std::transform(
        tile_indexes.begin() + 1,
        tile_indexes.begin() + count,
        tile_indexes.begin() + 2,
        std::back_inserter(icons_map),
        [&](auto first, auto last) {
            last = last > 0 ? last : tile_indexes.size();
            return std::vector(
                tile_indexes.begin() + first,
                tile_indexes.begin() + last
            );
        }
    );

    return icons_map;
}

using SSet = std::vector<std::vector<uint8_t>>;
using RPal = std::vector<std::vector<uint8_t>>;
using RTbl = std::vector<uint8_t>;

ICN::Tile::Info
read_sinf_chunk(std::fstream &input) {
    using namespace std::literals;

    io::check(input, []{ return "SINF"sv; });

    const auto sinf_chunk_size = io::readBEInteger<4>(input);

    const auto width  = io::readLEInteger<1>(input);
    const auto height = io::readLEInteger<1>(input);
    const auto shift  = io::readLEInteger<1>(input);

    const auto bit_per_pixel = io::readLEInteger<1>(input);

    return ICN::Tile::Info{
        width  << shift,
        height << shift,
        bit_per_pixel
    };
}

SSet
read_sset_chunk(std::fstream &input, const ICN::Tile::Info &info) {
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
read_rpal_chunk(std::fstream &input, const ICN::Tile::Info &info) {
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
read_rtbl_chunk(std::fstream &input, const ICN::Tile::Info &info) {
    using namespace std::literals;

    io::check(input, []{ return "RTBL"sv; });

    const auto rtbl_chunk_size = io::readBEInteger<4>(input);
    const auto rtbl = io::readData<uint8_t>(input, rtbl_chunk_size);

    return rtbl;
}

std::tuple<size_t, size_t>
icon_shape_from_index(unsigned int index, size_t size) {
    std::tuple<size_t, size_t> wh;
    switch (index) {
    case 0:
    case 1:
    case 2:
    case 3:
        wh = std::make_tuple(3, 2);
        break;
    case 4:
        wh = std::make_tuple(4, 2);
        break;
    case 5:
        wh = std::make_tuple(15, 5);
        break;
    case 6:
        wh = std::make_tuple(4, 4);
        break;
    case 8:
        wh = std::make_tuple(27, 3);
        break;
    case 10:
        wh = std::make_tuple(3, 12);
        break;
    case 11:
        wh = std::make_tuple(2, 12);
        break;
    case 12:
    case 13:
        wh = std::make_tuple(3, 16);
        break;
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
        wh = std::make_tuple(2, 8);
        break;
    case 19:
        wh = std::make_tuple(3, 30);
        break;
    case 20:
    case 21:
        wh = std::make_tuple(3, 20);
        break;
    case 22:
    case 23:
        wh = std::make_tuple(1, 10);
        break;
    case 24:
        wh = std::make_tuple(2, 8);
        break;
    case 25:
        wh = std::make_tuple(2, 12);
        break;
    default:
        wh = std::make_tuple(size, 1);
        break;
    }

    std::cout
        << "ICON#" << index
        << " size=" << size
        << " w=" << std::get<0>(wh)
        << " h=" << std::get<1>(wh)
    << std::endl;

    assert(std::get<0>(wh)*std::get<1>(wh) <= size);

    return wh;
}
} // namespace

ICN::Tile::Tile(
    const Palette &palette,
    const Info &info,
    const std::vector<uint8_t> &data,
    const std::vector<uint8_t> &paletteIndexes)
    : palette_{palette}
    , info_{info}
    , data_{data}
    , paletteIndexes_{paletteIndexes} {
}

std::size_t
ICN::Tile::getWidth() const {
    return info_.width;
}

std::size_t
ICN::Tile::getHeight() const {
    return info_.width;
}

Color
ICN::Tile::getPixel(std::size_t x, std::size_t y) const {
    const auto index = y*getWidth() + x;

    const auto k = (index*info_.bitPerPixels)/8;
    const auto s = info_.bitPerPixels - (index*info_.bitPerPixels)%8;
    const auto p = (data_[k] >> s) & ((1 << info_.bitPerPixels) - 1);

    return palette_[paletteIndexes_[p]];
}

ICN::Icon::Icon(std::size_t width, std::size_t height, const Tile::Info &tile_info)
    : width_{width}
    , height_{height}
    , tileInfo_{tile_info} {
}

std::size_t
ICN::Icon::getWidth() const {
    return width_*tileInfo_.width;
}

std::size_t
ICN::Icon::getHeight() const {
    return height_*tileInfo_.height;
}

Color 
ICN::Icon::getPixel(std::size_t x, std::size_t y) const {
    assert(x < getWidth());
    assert(y < getHeight());

    const auto tile_index = x/tileInfo_.width + y/tileInfo_.height*width_;
    const auto &tile = tiles_[tile_index];

    x = x%tileInfo_.width;
    y = y%tileInfo_.height;

    return tile.getPixel(x, y);
}

ICN::ICN(const Palette &palette)
    : palette_{palette} {
}

std::optional<ICN>
ICN::load(
    const std::string &map_filepath,
    const std::string &icn_filepath,
    const Palette &palette) {
    using namespace std::literals;
    std::fstream map_input(map_filepath, std::ios::binary|std::ios::in);
    std::fstream icn_input(icn_filepath, std::ios::binary|std::ios::in);

    auto icn = std::make_optional<ICN>(palette);

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

        icn->iconsTilesMapping_ = read_icons_map(map_input);
    } catch (...) {
        return std::nullopt;
    }

    return icn;
}

std::size_t
ICN::getTileCount() const {
    return tilesDataTable_.size();
}

ICN::Tile
ICN::getTile(std::size_t tile_index) const {
    assert(tilesPaletteIndexesTable_.size() == getTileCount());
    return Tile(
        palette_,
        tileInfo_,
        tilesDataTable_[tile_index],
        tilesPaletteIndexes_[tilesPaletteIndexesTable_[tile_index]]
    );
}

std::size_t
ICN::getIconCount() const {
    return iconsTilesMapping_.size();
}

ICN::Icon
ICN::getIcon(std::size_t icon_index) const {
    const auto icon_mapping = iconsTilesMapping_[icon_index];
    const auto [icon_width, icon_height] = icon_shape_from_index(icon_index, icon_mapping.size());
    Icon icon(icon_width, icon_height, tileInfo_);

    std::transform(
        icon_mapping.begin(),
        icon_mapping.end(),
        std::back_inserter(icon.tiles_),
        [&] (auto tile_index) {
            return getTile(tile_index);
        }
    );

    return icon;
}

ICN::TileIterator
ICN::tilesBegin() const {
    using namespace std::placeholders;
    return TileIterator(std::bind(&ICN::getTile, this, _1), 0);
}

ICN::TileIterator 
ICN::tilesEnd() const {
    return TileIterator([](auto tile_index) -> Tile {
        throw std::out_of_range("tile index out of range");
    }, getTileCount());
}

ICN::IconIterator
ICN::iconsBegin() const {
    using namespace std::placeholders;
    return IconIterator(std::bind(&ICN::getIcon, this, _1), 0);
}

ICN::IconIterator
ICN::iconsEnd() const {
    return IconIterator([](auto icon_index) -> Icon {
        throw std::out_of_range("icon index out of range");
    }, getIconCount());
}

} // namespace nr::dune2

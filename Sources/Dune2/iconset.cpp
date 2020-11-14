#include "iconset.hpp"
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

Iconset::Icon::Icon(std::size_t width, std::size_t height, const Tileset::Tile::Info &tile_info)
    : width_{width}
    , height_{height}
    , tileInfo_{tile_info} {
}

std::size_t
Iconset::Icon::getWidth() const {
    return width_*tileInfo_.width;
}

std::size_t
Iconset::Icon::getHeight() const {
    return height_*tileInfo_.height;
}

std::size_t
Iconset::Icon::getPixel(std::size_t x, std::size_t y) const {
    assert(x < getWidth());
    assert(y < getHeight());

    const auto tile_index = x/tileInfo_.width + y/tileInfo_.height*width_;
    const auto &tile = tiles_[tile_index];

    x = x%tileInfo_.width;
    y = y%tileInfo_.height;

    return tile.getPixel(x, y);
}

Iconset::Iconset(const Tileset &tileset)
    : tileset_{tileset} {
}

std::optional<Iconset>
Iconset::load(
    const std::string &map_filepath,
    const Tileset &tileset) {
    using namespace std::literals;
    std::fstream map_input(map_filepath, std::ios::binary|std::ios::in);

    auto iconset = std::make_optional<Iconset>(tileset);

    try {
        iconset->iconsTilesMapping_ = read_icons_map(map_input);
    } catch (...) {
        return std::nullopt;
    }

    return iconset;
}

std::size_t
Iconset::getIconCount() const {
    return iconsTilesMapping_.size();
}

Iconset::Icon
Iconset::getIcon(std::size_t icon_index) const {
    using namespace std::placeholders;

    const auto icon_mapping = iconsTilesMapping_[icon_index];
    const auto [icon_width, icon_height] = icon_shape_from_index(icon_index, icon_mapping.size());
    Icon icon(icon_width, icon_height, tileset_.getTileInfo());

    std::transform(
        icon_mapping.begin(),
        icon_mapping.end(),
        std::back_inserter(icon.tiles_),
        std::bind(&Tileset::getTile, &tileset_, _1)
    );

    return icon;
}

Iconset::IconIterator
Iconset::begin() const {
    using namespace std::placeholders;
    return IconIterator(std::bind(&Iconset::getIcon, this, _1), 0);
}

Iconset::IconIterator
Iconset::end() const {
    return IconIterator([](auto icon_index) -> Icon {
        throw std::out_of_range("icon index out of range");
    }, getIconCount());
}

} // namespace nr::dune2

#include "iconset.hpp"
#include "io.hpp"

#include <fstream>

namespace nr::dune2 {

namespace {

Iconset::Icon
icon_shape_from_index(std::size_t icon_index, std::vector<std::size_t> &&tile_indexes) {
    const auto size = tile_indexes.size();
    std::tuple<size_t, size_t> shape;

    switch (icon_index) {
    case 0:
    case 1:
    case 2:
    case 3:
        shape = std::make_tuple(3, 2);
        break;
    case 4:
        shape = std::make_tuple(4, 2);
        break;
    case 5:
        shape = std::make_tuple(15, 5);
        break;
    case 6:
        shape = std::make_tuple(4, 4);
        break;
    case 8:
        shape = std::make_tuple(27, 3);
        break;
    case 10:
        shape = std::make_tuple(3, 12);
        break;
    case 11:
        shape = std::make_tuple(2, 12);
        break;
    case 12:
    case 13:
        shape = std::make_tuple(3, 16);
        break;
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
        shape = std::make_tuple(2, 8);
        break;
    case 19:
        shape = std::make_tuple(3, 30);
        break;
    case 20:
    case 21:
        shape = std::make_tuple(3, 20);
        break;
    case 22:
    case 23:
        shape = std::make_tuple(1, 10);
        break;
    case 24:
        shape = std::make_tuple(2, 8);
        break;
    case 25:
        shape = std::make_tuple(2, 12);
        break;
    default:
        shape = std::make_tuple(size, 1);
        break;
    }

    // std::cout
    //     << "ICON#" << index
    //     << " size=" << size
    //     << " w=" << std::get<0>(wh)
    //     << " h=" << std::get<1>(wh)
    // << std::endl;

    const auto [columns, rows] = shape;

    assert(columns*rows <= size);
    return Iconset::Icon(columns, rows, std::move(tile_indexes));
}
} // namespace

void
Iconset::loadFromMAP(const std::string &map_path) {
    std::fstream map_input(map_path, std::ios::binary|std::ios::in);
    std::vector<std::size_t> tile_indexes;

    while (!map_input.eof()) {
        tile_indexes.emplace_back(io::readLEInteger<2>(map_input));
    }

    const auto count = tile_indexes[0];

    std::transform(
        tile_indexes.begin() + 1,
        tile_indexes.begin() + count,
        tile_indexes.begin() + 2,
        std::back_inserter(icons_),
        [&, icon_index = 0u](auto first, auto last) mutable {
            last = last > 0 ? last : tile_indexes.size();
            return icon_shape_from_index(
                icon_index++,
                std::vector(tile_indexes.begin() + first, tile_indexes.begin() + last)
            );
        }
    );
}

} // namespace nr::dune2

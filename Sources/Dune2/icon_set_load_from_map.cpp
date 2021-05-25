#include "icon_set.hpp"
#include "io.hpp"

#include <fstream>

namespace nr::dune2 {

namespace {

using ImageIndexList = std::vector<std::size_t>;
using ImageIndexListIterator = ImageIndexList::const_iterator;
using ImageIndexRangeList = std::vector<std::tuple<
        ImageIndexListIterator,
        ImageIndexListIterator
    >
>;

using ShapeList = std::vector<std::tuple<
        size_t,
        size_t
    >
>;

void
icons_from_index(
    std::size_t icon_index,
    ImageIndexListIterator first,
    ImageIndexListIterator last,
    std::vector<IconSet::Icon> &icons
) {
    ShapeList shapes;

    switch (icon_index) {
    case 10:
        shapes = ShapeList( 4, std::make_tuple(3, 3));
        break;
    case 11:
    case 25:
        shapes = ShapeList( 6, std::make_tuple(2, 2));
        break;
    case 12:
    case 13:
        shapes = ShapeList( 8, std::make_tuple(3, 2));
        break;
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 24:
        shapes = ShapeList( 4, std::make_tuple(2, 2));
        break;
    case 19:
        shapes = ShapeList(10, std::make_tuple(3, 3));
        break;
    case 20:
    case 21:
        shapes = ShapeList(10, std::make_tuple(3, 2));
        break;
    default:
        shapes = ShapeList(std::distance(first, last), std::make_tuple(1, 1));
        break;
    }

    std::for_each(
        shapes.begin(),
        shapes.end(),
        [&](const auto shape) mutable {
            const auto [columns, rows] = shape;
            const auto count = columns*rows;

            assert(first + count <= last);
            icons.emplace_back(IconSet::Icon(
                columns, rows,
                ImageIndexList(first, first + count)
            ));

            first += count;
        }
    );
}
} // namespace

void
IconSet::loadFromMAP(const std::filesystem::path &map_path) {
    std::fstream map_input(map_path, std::ios::binary|std::ios::in);
    ImageIndexList indexes;

    while (!map_input.eof()) {
        indexes.emplace_back(io::readLEInteger<2>(map_input));
    }

    ImageIndexRangeList ranges;
    std::transform(
        indexes.begin() + 1,
        indexes.begin() + indexes[0],
        indexes.begin() + 2,
        std::back_inserter(ranges),
        [&](auto first, auto last) {
            last = last > 0 ? last : indexes.size();
            return std::make_tuple(
                indexes.begin() + first,
                indexes.begin() + last
            );
        }
    );

    std::for_each(
        ranges.begin(),
        ranges.end(),
        [&, icon_index = 0u](const auto &range) mutable {
            const auto [first, last] = range;
            icons_from_index(
                icon_index++,
                first,
                last,
                icons_
            );
        }
    );
}

} // namespace nr::dune2

#include "iconset.hpp"

namespace nr::dune2 {

Iconset::Icon::Surface::Surface(
    const std::size_t col,
    const std::size_t row,
    std::vector<Tileset::Tile> &&tiles)
    : column_{col}
    , row_{row}
    , tiles_{std::move(tiles)} {
}

size_t
Iconset::Icon::Surface::getWidth() const {
    return column_*tiles_.front().getWidth();
}

size_t
Iconset::Icon::Surface::getHeight() const {
    return row_*tiles_.front().getHeight();
}

size_t
Iconset::Icon::Surface::getPixel(size_t x, size_t y) const {
    assert((x < getWidth()) && (y < getHeight()));

    const auto w = tiles_.front().getWidth();
    const auto h = tiles_.front().getHeight();

    const auto tile_index = x/w + (y/h)*column_;
    const auto &tile = tiles_.at(tile_index);

    x = x%w;
    y = y%h;

    return tile.getPixel(x, y);
}

size_t
Iconset::Icon::getColumnCount() const {
    return columns_;
}

size_t
Iconset::Icon::getRowCount() const {
    return rows_;
}

Iconset::Icon::Surface
Iconset::Icon::getSurface(const Tileset &tileset) const {
    using namespace std::placeholders;
    std::vector<Tileset::Tile> tiles;
    std::transform(
        tiles_.begin(),
        tiles_.end(),
        std::back_inserter(tiles),
        std::bind(&Tileset::getTile, tileset, _1)
    );
    return Surface(columns_, rows_, std::move(tiles));
}

} // namespace nr::dune2

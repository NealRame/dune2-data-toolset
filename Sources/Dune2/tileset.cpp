#include "tileset.hpp"
#include "io.hpp"

#include <stdexcept>
#include <fstream>
#include <iostream>

namespace nr::dune2 {

size_t
Tileset::Tile::getPixel(size_t x, size_t y) const {
    assert(x < getWidth());
    assert(y < getHeight());

    const auto index = y*getWidth() + x;
    const auto pixel = static_cast<unsigned char>(data_[index]);
    if (hasRemapTable()) {
        return static_cast<unsigned char>(dataRemapTable_[pixel]);
    }
    return pixel;
}

const std::string &
Tileset::Tile::getData() const {
    return data_;
}

const std::string &
Tileset::Tile::getRemapTableData() const {
    return dataRemapTable_;
}

bool
Tileset::Tile::hasRemapTable() const {
    return dataRemapTable_.size() > 0;
}

size_t
Tileset::getTileCount() const {
    return tiles_.size();
}

const Tileset::Tile &
Tileset::getTile(size_t tile_index) const {
    return tiles_[tile_index];
}

Tileset::TileIterator
Tileset::begin() const {
    return tiles_.begin();
}

Tileset::TileIterator 
Tileset::end() const {
    return tiles_.end();
}

} // namespace nr::dune2

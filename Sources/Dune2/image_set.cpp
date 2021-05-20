#include "image_set.hpp"
#include "io.hpp"

#include <stdexcept>
#include <fstream>
#include <iostream>

namespace nr::dune2 {

size_t
ImageSet::Image::getPixel(size_t x, size_t y) const {
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
ImageSet::Image::getData() const {
    return data_;
}

const std::string &
ImageSet::Image::getRemapTableData() const {
    return dataRemapTable_;
}

bool
ImageSet::Image::hasRemapTable() const {
    return dataRemapTable_.size() > 0;
}

size_t
ImageSet::getTileCount() const {
    return tiles_.size();
}

const ImageSet::Image &
ImageSet::getTile(size_t tile_index) const {
    return tiles_[tile_index];
}

ImageSet::TileIterator
ImageSet::begin() const {
    return tiles_.begin();
}

ImageSet::TileIterator
ImageSet::end() const {
    return tiles_.end();
}

} // namespace nr::dune2

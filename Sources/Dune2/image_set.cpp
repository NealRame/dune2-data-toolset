#include "image_set.hpp"
#include "io.hpp"

#include <stdexcept>
#include <fstream>
#include <iostream>

namespace nr::dune2 {

size_t
ImageSet::getImageCount() const {
    return tiles_.size();
}

const Image &
ImageSet::getImage(size_t tile_index) const {
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

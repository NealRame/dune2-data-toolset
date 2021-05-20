#include "image.hpp"

namespace nr::dune2 {

size_t
Image::getPixel(size_t x, size_t y) const {
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
Image::getData() const {
    return data_;
}

const std::string &
Image::getRemapTableData() const {
    return dataRemapTable_;
}

bool
Image::hasRemapTable() const {
    return dataRemapTable_.size() > 0;
}

} // namespace nr::dune2

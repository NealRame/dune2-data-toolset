#pragma once

#include <Dune2/surface.hpp>

#include <filesystem>
#include <string>

namespace nr::dune2 {
/// ### class nr::dune2::ImageSet::Image
/// An image loaded from an `.icn` or an `.shp` file.
/// `nr::dune2::ImageSet::Image` implements `nr::dune2::Surface`.
/// See [`nr::dune2::Surface`](/docs/nr/dune2/surface) for more details.
class Image: public Surface {
    friend class ImageSet;

public:
    Image()
        : width_{0}
        , height_{0} {
    }

    template <typename T>
    Image(size_t width, size_t height, T &&data)
        : width_{width}
        , height_{height}
        , data_{std::forward<T>(data)} {
    }

    template <typename T, typename U>
    Image(size_t width, size_t height, T &&data, U &&data_remap_table)
        : width_{width}
        , height_{height}
        , data_{std::forward<T>(data)}
        , dataRemapTable_{std::forward<U>(data_remap_table)} {
    }

public:
    /// ### method `nr::dune2::ImageSet::Image.getWidth`
    /// See [`nr::dune2::Surface.getWidth`](/docs/nr/dune2/surface#getWidth)
    /// for more details.
    virtual size_t getWidth() const override
    { return width_; }

    /// ### method `nr::dune2::ImageSet::Image.getHeight`
    /// See [`nr::dune2::Surface.getHeight`](/docs/nr/dune2/surface#getHeight)
    /// for more details.
    virtual size_t getHeight() const override
    { return height_; }

    /// ### method `nr::dune2::ImageSet::Image.getPixel`
    /// See [`nr::dune2::Surface.getPixel`](/docs/nr/dune2/surface#getPixel)
    /// for more details.
    virtual size_t getPixel(size_t, size_t) const override;

    /// ### method `nr::dune2::ImageSet::Image::getData`
    /// #### Return
    /// - `const std::string &` - a reference on the tile's raw data.
    const std::string &getData() const;

    /// ### method `nr::dune2::ImageSet::Image::getRemapTableData`
    /// #### Return
    /// - `const std::string &` - a reference on the tile's remap table raw
    /// data.
    const std::string &getRemapTableData() const;

    /// ### method `nr::dune2::ImageSet::Image::hasRemapTable`
    /// #### Return
    /// - `bool` - `true` if tile has a remap table.
    bool hasRemapTable() const;

private:
    size_t width_;
    size_t height_;
    std::string data_;
    std::string dataRemapTable_;
};
}
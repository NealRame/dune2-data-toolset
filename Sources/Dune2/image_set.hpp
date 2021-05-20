#pragma once

#include <Dune2/surface.hpp>

#include <filesystem>
#include <string>
#include <vector>

#include <rapidjson/document.h>

namespace nr::dune2 {
class ImageSet {
public:
    /// ### class nr::dune2::ImageSet::Image
    /// An image loaded from an `.icn` or an `.shp` file.
    /// `nr::dune2::ImageSet::Image` implements `nr::dune2::Surface`.
    /// See [`nr::dune2::Surface`](/docs/nr/dune2/surface) for more details.
    class Image: public Surface {
        friend class ImageSet;

    public:
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

    /// ### class `nr::dune2::ImageSet::TileIterator`
    /// An input iterator to iterate throught tiles.
    using TileIterator = std::vector<Image>::const_iterator;

public:
    /// ### method `nr::dune2::ImageSet::loadFromICN`
    /// Load tiles from given `.icn` files.
    /// #### Parameters
    /// - `const std::filesystem::path &icn_path` - a path to `*.icn` file
    void loadFromICN(const std::filesystem::path &);

    /// ### method `nr::dune2::ImageSet::loadFromSHP`
    /// Load tiles from given `.shp` files.
    /// #### Parameters
    /// - `const std::filesystem::path &shp_path` - a path to `*.shp` file
    void loadFromSHP(const std::filesystem::path &);

    /// ### method `nr::dune2::ImageSet::loadFromJSON`
    /// Load tiles from the given _JSON_ value.
    /// #### Parameters
    /// - `const std::filesystem::path &` - a path to `*.json` file
    void loadFromJSON(const std::filesystem::path &);

public:
    /// ### method `nr::dune2::ImageSet::toJSON`
    /// Transform this tileset to _JSON_ document
    /// #### Return
    /// `rapidjson::Document` - a json document
    rapidjson::Document toJSON() const;

public:
    /// ### method `nr::dune2::ImageSet.getName`
    /// #### Return
    /// `const std::string &` - this `ImageSet` name.
    const std::string &getName() const
    { return name_; }

    /// ### method nr::dune2::ImageSet.setName`
    /// #### Parameters
    /// - `const std::string &name` - set the name of the tileset
    /// #### Return
    /// `ImageSet &` - this `ImageSet`.
    ImageSet &setName(const std::string &name) {
        name_ = name;
        return *this;
    }

    /// ### method `nr::dune2::ImageSet.getTileCount`
    /// #### Return
    /// `size_t` - the number of tiles.
    size_t getTileCount() const;

    /// ### method `nr::dune2::ImageSet.getTile`
    /// #### Parameters
    /// - `tile_index` - the tile index.
    /// #### Return
    /// `ImageSet::Image` - a tile _tiles[tile_index]_.
    const Image &getTile(size_t tile_index) const;

    /// ### method `nr::dune2::ImageSet.tilesBegin`
    /// #### Return
    /// `ImageSet::TileIterator` - an iterator on the first tile.
    TileIterator begin() const;

    /// ### method `nr::dune2::ImageSet.tilesEnd`
    /// #### Return
    /// `ImageSet::TileIterator` - an iterator on the last tile.
    TileIterator end() const;

public:
    template <typename T>
    void push_back(T &&tile) {
        tiles_.push_back(std::forward<T>(tile));
    }

private:
    std::string name_;
    std::vector<Image> tiles_;
};
} // namespace nr::dune2

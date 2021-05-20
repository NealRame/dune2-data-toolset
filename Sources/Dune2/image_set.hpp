#pragma once

#include <Dune2/image.hpp>

#include <filesystem>
#include <string>
#include <vector>

#include <rapidjson/document.h>

namespace nr::dune2 {
class ImageSet {
public:
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

    /// ### method `nr::dune2::ImageSet.getImageCount`
    /// #### Return
    /// `size_t` - the number of tiles.
    size_t getImageCount() const;

    /// ### method `nr::dune2::ImageSet.getImage`
    /// #### Parameters
    /// - `tile_index` - the tile index.
    /// #### Return
    /// `ImageSet::Image` - a tile _tiles[tile_index]_.
    const Image &getImage(size_t tile_index) const;

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
    void push_back(T &&image) {
        tiles_.push_back(std::forward<T>(image));
    }

private:
    std::string name_;
    std::vector<Image> tiles_;
};
} // namespace nr::dune2

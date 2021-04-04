#pragma once

#include <Dune2/surface.hpp>

#include <filesystem>
#include <string>
#include <vector>

namespace nr::dune2 {
class Tileset {
public:
    /// ### class nr::dune2::Tileset::Tile
    /// A tile loaded from an `.icn` file.
    /// `nr::dune2::Tileset::Tile` implements `nr::dune2::Surface`.
    /// See [`nr::dune2::Surface`](/docs/nr/dune2/surface) for more details.
    class Tile: public Surface {
        friend class Tileset;

    public:
        template <typename T>
        Tile(size_t width, size_t height, T &&data)
            : width_{width}
            , height_{height}
            , data_{std::forward<T>(data)} {
        }

        template <typename T, typename U>
        Tile(size_t width, size_t height, T &&data, U &&data_remap_table)
            : width_{width}
            , height_{height}
            , data_{std::forward<T>(data)}
            , dataRemapTable_{std::forward<U>(data_remap_table)} {
        }

    public:
        /// ### method `nr::dune2::Tileset::Tile.getWidth`
        /// See [`nr::dune2::Surface.getWidth`](/docs/nr/dune2/surface#getWidth)
        /// for more details.
        virtual size_t getWidth() const override
        { return width_; }

        /// ### method `nr::dune2::Tileset::Tile.getHeight`
        /// See [`nr::dune2::Surface.getHeight`](/docs/nr/dune2/surface#getHeight)
        /// for more details.
        virtual size_t getHeight() const override
        { return height_; }

        /// ### method `nr::dune2::Tileset::Tile.getPixel`
        /// See [`nr::dune2::Surface.getPixel`](/docs/nr/dune2/surface#getPixel)
        /// for more details.
        virtual size_t getPixel(size_t, size_t) const override;

        /// ### method `nr::dune2::Tileset::Tile::getData`
        /// #### Return
        /// - `const std::string &` - a reference on the tile's raw data.
        const std::string &getData() const;

        /// ### method `nr::dune2::Tileset::Tile::getRemapTableData`
        /// #### Return
        /// - `const std::string &` - a reference on the tile's remap table raw
        /// data.
        const std::string &getRemapTableData() const;

        /// ### method `nr::dune2::Tileset::Tile::hasRemapTable`
        /// #### Return
        /// - `bool` - `true` if tile has a remap table.
        bool hasRemapTable() const;

    private:
        size_t width_;
        size_t height_;
        std::string data_;
        std::string dataRemapTable_;
    };

    /// ### class `nr::dune2::Tileset::TileIterator`
    /// An input iterator to iterate throught tiles.
    using TileIterator = std::vector<Tile>::const_iterator;

public:
    /// ### method `nr::dune2::Tileset::loadFromICN`
    /// Load tiles from given `.icn` files.
    /// #### Parameters
    /// * `const std::filesystem::path &icn_path` - a path to `*.icn` file
    void loadFromICN(const std::filesystem::path &icn_path);

    /// ### method `nr::dune2::Tileset::loadFromSHP`
    /// Load tiles from given `.shp` files.
    /// #### Parameters
    /// * `const std::filesystem::path &shp_path` - a path to `*.shp` file
    void loadFromSHP(const std::filesystem::path &shp_path);

public:
    /// ### method `nr::dune2::Tileset.getTileInfo`
    const std::string &getName() const
    { return name_; }

    /// ### method `nr::dune2::Tileset.getTileCount`
    /// #### Return
    /// `size_t` - the number of tiles.
    size_t getTileCount() const;

    /// ### method `nr::dune2::Tileset.getTile`
    /// #### Parameters
    /// * `tile_index` - the tile index.
    /// #### Return
    /// `Tileset::Tile` - a tile _tiles[tile_index]_.
    const Tile &getTile(size_t tile_index) const;

    /// ### method `nr::dune2::Tileset.tilesBegin`
    /// #### Return
    /// `Tileset::TileIterator` - an iterator on the first tile.
    TileIterator begin() const;

    /// ### method `nr::dune2::Tileset.tilesEnd`
    /// #### Return
    /// `Tileset::TileIterator` - an iterator on the last tile.
    TileIterator end() const;

public:
    template <typename T>
    void push_back(T &&tile) {
        tiles_.push_back(std::forward<T>(tile));
    }

private:
    std::string name_;
    std::vector<Tile> tiles_;
};
} // namespace nr::dune2

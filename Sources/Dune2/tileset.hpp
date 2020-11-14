#pragma once

#include <Dune2/iterators.hpp>
#include <Dune2/palette.hpp>
#include <Dune2/surface.hpp>

#include <optional>
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
        struct Info {
            /// #### attribute `width`
            /// The pixels width of a Tile.
            std::size_t width;

            /// #### attribute `height`
            /// The pixels height of a Tile.
            std::size_t height;

            /// #### attribute `bitPerPixels`
            /// The number of bits used to store one pixel.
            std::size_t bitPerPixels;

            /// #### method `Tile.getTileSize`
            /// ##### Return
            /// `std::size_t` - the number of bytes used to store a tile.
            std::size_t getTileSize() const
            { return (width*height*bitPerPixels)/8; }

            /// #### method `Tile.getPaletteSize`
            /// ##### Return
            /// `std::size_t` - the number of colors used in a tile.
            std::size_t getPaletteSize() const
            { return 1<<bitPerPixels; }
        };

    public:
        /// #### method `nr::dune2::Tileset::Tile.getWidth`
        /// See [`nr::dune2::Surface.getWidth`](/docs/nr/dune2/surface#getWidth)
        /// for more details.
        virtual std::size_t getWidth() const override;

        /// #### method `nr::dune2::Tileset::Tile.getHeight`
        /// See [`nr::dune2::Surface.getHeight`](/docs/nr/dune2/surface#getHeight)
        /// for more details.
        virtual std::size_t getHeight() const override;

        /// #### method `nr::dune2::Tileset::Tile.getPixel`
        /// See [`nr::dune2::Surface.getPixel`](/docs/nr/dune2/surface#getPixel)
        /// for more details.
        virtual Color getPixel(std::size_t x, std::size_t y) const override;

    private:
        Tile(const Palette &, const Info &, const std::vector<uint8_t> &, const std::vector<uint8_t> &);

    private:
        const Palette &palette_;
        const Info &info_;
        const std::vector<uint8_t> &data_;
        const std::vector<uint8_t> &paletteIndexes_;
    };

    /// ### class `nr::dune2::Tileset::TileIterator`
    /// An input iterator to iterate throught tiles.
    using TileIterator = Iterator<Tile>;

public:
    Tileset(const Palette &);

public:
    /// ### static method `nr::dune2::Tileset::load`
    /// Load tiles and icons from a pair of `.map` and `.icn` files.
    /// #### Parameters
    /// * `const std::string &icn_path` - a path to `*.icn` file
    /// * `const Palette &` - a palette
    /// #### Returns
    /// `std::optional<Tileset>`
    /// * an initialized optional<Tileset> if load succeed
    /// * `std::nullopt` otherwise
    static std::optional<Tileset> load(
        const std::string &icn_path,
        const Palette &
    );

public:
    /// ### method `nr::dune2::Tileset.getTileCount`
    /// #### Return
    /// `std::size_t` - the number of tiles.
    std::size_t getTileCount() const;

    /// ### method `nr::dune2::Tileset.getTile`
    /// #### Parameters
    /// * `tile_index` - the tile index.
    /// #### Return
    /// `Tileset::Tile` - a tile _tiles[tile_index]_.
    Tile getTile(std::size_t tile_index) const;

    /// ### method `nr::dune2::Tileset.tilesBegin`
    /// #### Return
    /// `Tileset::TileIterator` - an iterator on the first tile.
    TileIterator begin() const;

    /// ### method `nr::dune2::Tileset.tilesEnd`
    /// #### Return
    /// `Tileset::TileIterator` - an iterator on the last tile.
    TileIterator end() const;

    /// ### method `nr::dune2::Tileset.getTileInfo`
    /// #### Return
    /// `nr::dune2::Tilset::Tile::Info` - tiles info
    const Tile::Info &getTileInfo() const
    { return tileInfo_; }

private:
    const Palette &palette_;
    Tile::Info tileInfo_;
    std::vector<std::vector<uint8_t>> tilesDataTable_;
    std::vector<uint8_t> tilesPaletteIndexesTable_;
    std::vector<std::vector<uint8_t>> tilesPaletteIndexes_;
};
} // namespace nr::dune2

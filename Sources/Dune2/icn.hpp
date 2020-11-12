#pragma once

#include <Dune2/palette.hpp>
#include <Dune2/surface.hpp>

#include <functional>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace nr::dune2 {
class ICN {
public:
    /// ### class nr::dune2::ICN::Tile
    /// A tile loaded from an `.icn` file.
    /// `nr::dune2::ICN::Tile` implements `nr::dune2::Surface`.
    /// See [`nr::dune2::Surface`](/docs/nr/dune2/surface) for more details.
    class Tile: public Surface {
        friend class ICN;

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
        /// #### method `nr::dune2::ICN::Tile.getWidth`
        /// See [`nr::dune2::Surface.getWidth`](/docs/nr/dune2/surface#getWidth)
        /// for more details.
        virtual std::size_t getWidth() const override;

        /// #### method `nr::dune2::ICN::Tile.getHeight`
        /// See [`nr::dune2::Surface.getHeight`](/docs/nr/dune2/surface#getHeight)
        /// for more details.
        virtual std::size_t getHeight() const override;

        /// #### method `nr::dune2::ICN::Tile.getPixel`
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

    /// ### class nr::dune2::ICN::Icon
    /// An icon loaded from an `*.map` file.
    /// Icons is a coherent group of tiles (building, landscape, walls, etc).
    /// `nr::dune2::ICN::Icon` implements `nr::dune2::Surface`.
    /// See [`nr::dune2::Surface`](/docs/nr/dune2/surface) for more details.
    class Icon: public Surface {
        friend class ICN;
    
    public:
        /// #### method `nr::dune2::ICN::Icon.getWidth`
        /// See [`nr::dune2::Surface.getWidth`](/docs/nr/dune2/surface#getWidth)
        /// for more details.
        virtual std::size_t getWidth() const override;

        /// #### method `nr::dune2::ICN::Icon.getHeight`
        /// See [`nr::dune2::Surface.getHeight`](/docs/nr/dune2/surface#getHeight)
        /// for more details.
        virtual std::size_t getHeight() const override;

        /// #### method `nr::dune2::ICN::Icon::getPixel`
        /// See [`nr::dune2::Surface.getPixel`](/docs/nr/dune2/surface#getPixel)
        /// for more details.
        virtual Color getPixel(std::size_t x, std::size_t y) const override;

    private:
        Icon(std::size_t, std::size_t, const Tile::Info &);

    private:
        std::size_t width_;
        std::size_t height_;
        const Tile::Info &tileInfo_;
        std::vector<Tile> tiles_;
    };

    template<typename T>
    struct Iterator {
        friend ICN;

    public:
        using difference_type = size_t;
        using value_type = T;
        using reference_type = value_type;
        using iterator_category = std::input_iterator_tag;

    public:
        using ItemGetter = std::function<T(std::size_t)>;

    public:
        bool operator==(const Iterator &rhs) {
            return index_ == rhs.index_;
        }

        bool operator!=(const Iterator &rhs)
        { return index_ != rhs.index_; }

        const T operator*() const
        { return getItem_(index_); }

        Iterator &operator++() {
            index_ += 1;
            return *this;
        }

    private:
        Iterator(ItemGetter &&item_getter, std::size_t index)
            : getItem_{item_getter}
            , index_{index} {
        }

    private:
        ItemGetter getItem_;
        std::size_t index_{0};
    };

    /// ### class `nr::dune2::ICN::TileIterator`
    /// An input iterator to iterate throught tiles.
    using TileIterator = Iterator<Tile>;

    /// ### class `nr::dune2::ICN::IconIterator`
    /// An input iterator to iterate throught icons.
    using IconIterator = Iterator<Icon>;

    template<typename T>
    struct Range {
        using iterator = T;

        iterator first, last;

        iterator begin() { return first; }
        iterator end() { return last; }
    };

    /// ### struct `nr::dune2::ICN::TileRange`
    /// A range of tiles.
    using TileRange = Range<TileIterator>;

    /// ### struct `nr::dune2::ICN::TileRange`
    /// A range of icons.
    using IconRange = Range<IconIterator>;

public:
    ICN(const Palette &);

public:
    /// ### static method `nr::dune2::ICN::load`
    /// Load tiles and icons from a pair of `.map` and `.icn` files.
    /// #### Parameters
    /// * `const std::string &map_filepath` - a file path
    /// * `const std::string &icn_filepath` - a file path
    /// * `const Palette &` - a palette
    /// #### Returns
    /// `std::optional<ICN>`
    /// * an initialized optional<ICN> if load succeed
    /// * `std::nullopt` otherwise
    static std::optional<ICN> load(
        const std::string &map_filepath,
        const std::string &icn_filepath,
        const Palette &
    );

public:
    /// ### method `nr::dune2::ICN.getTileCount`
    /// #### Return
    /// `std::size_t` - the number of tiles.
    std::size_t getTileCount() const;

    /// ### method `nr::dune2::ICN.getTile`
    /// #### Parameters
    /// * `tile_index` - the tile index.
    /// #### Return
    /// `ICN::Tile` - a tile _tiles[tile_index]_.
    Tile getTile(std::size_t tile_index) const;

    /// ### method `nr::dune2::ICN.tilesBegin`
    /// #### Return
    /// `ICN::TileIterator` - an iterator on the first tile.
    TileIterator tilesBegin() const;

    /// ### method `nr::dune2::ICN.tilesEnd`
    /// #### Return
    /// `ICN::TileIterator` - an iterator on the last tile.
    TileIterator tilesEnd() const;

    /// ### method `nr::dune2::ICN.tiles`
    /// #### Return
    /// `ICN::TileRange` - a range of TileIterator _[tilesBegin(), tilesEnd()[_.
    TileRange tiles() const
    { return TileRange{tilesBegin(), tilesEnd()}; }

public:
    /// ### method `nr::dune2::ICN.getIconCount`
    /// #### Return
    /// `std::size_t` - the number of icons.
    std::size_t getIconCount() const;

    /// ### method `nr::dune2::ICN.getTile`
    /// #### Parameters
    /// * `icon_index` - the icon index.
    /// #### Return
    /// `ICN::Tile` - a tile _tiles[tile_index]_.
    Icon getIcon(std::size_t icon_index) const;

    /// ### method `nr::dune2::ICN.iconsBegin`
    /// #### Return
    /// `ICN::IconIterator` - an iterator on the first icon.
    IconIterator iconsBegin() const;

    /// ### method `nr::dune2::ICN.iconsEnd`
    /// #### Return
    /// `ICN::IconIterator` - an iterator on the first icon.
    IconIterator iconsEnd() const;

    /// ### method `nr::dune2::ICN.icons`
    /// #### Return
    /// `ICN::TileRange` - a range of IconIterator _[iconsBegin(), iconsEnd()[_.
    IconRange icons() const
    { return IconRange{iconsBegin(), iconsEnd()}; }

private:
    const Palette &palette_;
    Tile::Info tileInfo_;
    std::vector<std::vector<uint8_t>> tilesDataTable_;
    std::vector<uint8_t> tilesPaletteIndexesTable_;
    std::vector<std::vector<uint8_t>> tilesPaletteIndexes_;
    std::vector<std::vector<unsigned int>> iconsTilesMapping_;
};
} // namespace nr::dune2

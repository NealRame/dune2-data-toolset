#pragma once

#include <Dune2/iterators.hpp>
#include <Dune2/palette.hpp>
#include <Dune2/surface.hpp>
#include <Dune2/tileset.hpp>

#include <optional>
#include <string>
#include <vector>

namespace nr::dune2 {
class Iconset {
public:
    /// ### class nr::dune2::Iconset::Icon
    /// An icon loaded from an `*.map` file.
    /// Icons is a coherent group of tiles (building, landscape, walls, etc).
    /// `nr::dune2::Iconset::Icon` implements `nr::dune2::Surface`.
    /// See [`nr::dune2::Surface`](/docs/nr/dune2/surface) for more details.
    class Icon: public Surface {
        friend class Iconset;
    
    public:
        /// #### method `nr::dune2::Iconset::Icon.getWidth`
        /// See [`nr::dune2::Surface.getWidth`](/docs/nr/dune2/surface#getWidth)
        /// for more details.
        virtual std::size_t getWidth() const override;

        /// #### method `nr::dune2::Iconset::Icon.getHeight`
        /// See [`nr::dune2::Surface.getHeight`](/docs/nr/dune2/surface#getHeight)
        /// for more details.
        virtual std::size_t getHeight() const override;

        /// #### method `nr::dune2::Iconset::Icon::getPixel`
        /// See [`nr::dune2::Surface.getPixel`](/docs/nr/dune2/surface#getPixel)
        /// for more details.
        virtual Color getPixel(std::size_t x, std::size_t y) const override;

    private:
        Icon(std::size_t, std::size_t, const Tileset::Tile::Info &);

    private:
        std::size_t width_;
        std::size_t height_;
        const Tileset::Tile::Info &tileInfo_;
        std::vector<Tileset::Tile> tiles_;
    };

    /// ### class `nr::dune2::Iconset::IconIterator`
    /// An input iterator to iterate throught icons.
    using IconIterator = Iterator<Icon>;

public:
    Iconset(const Tileset &);

public:
    /// ### static method `nr::dune2::Iconset::load`
    /// Load tiles and icons from a pair of `.map` and `.Iconset` files.
    /// #### Parameters
    /// * `const std::string &map_filepath` - a file path
    /// * `const Tileset &` - a tilset
    /// #### Returns
    /// `std::optional<Iconset>`
    /// * an initialized optional<Iconset> if load succeed
    /// * `std::nullopt` otherwise
    static std::optional<Iconset> load(
        const std::string &map_filepath,
        const Tileset &
    );

public:
    /// ### method `nr::dune2::Iconset.getIconCount`
    /// #### Return
    /// `std::size_t` - the number of icons.
    std::size_t getIconCount() const;

    /// ### method `nr::dune2::Iconset.getTile`
    /// #### Parameters
    /// * `icon_index` - the icon index.
    /// #### Return
    /// `Iconset::Tile` - a tile _tiles[tile_index]_.
    Icon getIcon(std::size_t icon_index) const;

    /// ### method `nr::dune2::Iconset.iconsBegin`
    /// #### Return
    /// `Iconset::IconIterator` - an iterator on the first icon.
    IconIterator begin() const;

    /// ### method `nr::dune2::Iconset.iconsEnd`
    /// #### Return
    /// `Iconset::IconIterator` - an iterator on the first icon.
    IconIterator end() const;

private:
    const Tileset &tileset_;
    std::vector<std::vector<unsigned int>> iconsTilesMapping_;
};
} // namespace nr::dune2

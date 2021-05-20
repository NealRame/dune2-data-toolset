#pragma once

#include <Dune2/surface.hpp>
#include <Dune2/image_set.hpp>

#include <rapidjson/document.h>

#include <string>
#include <vector>

namespace nr::dune2 {
class IconSet {
public:
    class Icon {
    public:
        using TileList = std::vector<Image>;
        using TileIndexList = std::vector<std::size_t>;

    public:
        class Surface : public nr::dune2::Surface {
            std::size_t column_;
            std::size_t row_;
            std::vector<Image> tiles_;

        public:
            Surface(std::size_t col, std::size_t row, TileList &&);

        public:
            /// ### method `nr::dune2::IconSet::Icon::Surface.getWidth`
            /// See [`nr::dune2::Surface.getWidth`](/docs/nr/dune2/surface#getWidth)
            /// for more details.
            virtual std::size_t getWidth() const override;

            /// ### method `nr::dune2::IconSet::Icon::Surface.getHeight`
            /// See [`nr::dune2::Surface.getHeight`](/docs/nr/dune2/surface#getHeight)
            /// for more details.
            virtual std::size_t getHeight() const override;

            /// ### method `nr::dune2::ImageSet::Image.getPixel`
            /// See [`nr::dune2::Surface.getPixel`](/docs/nr/dune2/surface#getPixel)
            /// for more details.
            virtual std::size_t getPixel(size_t, size_t) const override;
        };

    public:
        template<typename T>
        Icon(std::size_t cols, std::size_t rows, T &&tiles)
            : columns_{cols}
            , rows_{rows}
            , tiles_{std::forward<T>(tiles)} {
            static_assert(std::is_same_v<TileIndexList, std::remove_cv_t<T>>);
            assert(columns_*rows_ <= tiles_.size());
        }

    public:
        /// ### method `nr::dune2::IconSet::Icon::getTileIndexList`
        /// Get the tiles indexes
        /// #### Return
        /// - `const std::vector<std::size_t> &`
        const TileIndexList &getTileIndexList() const
        { return tiles_; }

        /// ### method `nr::dune2::IconSet::Icon.getColumnCount`
        /// Get the number of tiles on the width of the icon.
        /// #### Return
        /// - `std::size_t`
        std::size_t getColumnCount() const;

        /// ### method `nr::dune2::IconSet::Icon.getRowCount`
        /// Get the number of tiles on the height of the icon.
        /// #### Return
        /// - `std::size_t`
        std::size_t getRowCount() const;

        /// ### method `nr::dune2::IconSet::Icon.getSurface`
        /// Get an IconSurface
        /// #### Return
        /// - `nr::dune2::IconSet::Icon::Surface`
        Surface getSurface(const ImageSet &) const;

    private:
        std::size_t columns_;
        std::size_t rows_;
        TileIndexList tiles_;
    };

    /// ### class `nr::dune2::ImageSet::TileIterator`
    /// An input iterator to iterate throught tiles.
    using IconIterator = std::vector<Icon>::const_iterator;

public:
    /// ### method `nr::dune2::IconSet::loadFromMAP`
    /// Load tiles from given `.map` files.
    /// #### Parameters
    /// - `const std::filesystem::path &map_path` - a path to `*.map` file
    void loadFromMAP(const std::filesystem::path &);

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
    /// ### method `nr::dune2::IconSet.IconCount`
    /// #### Return
    /// `size_t` - the number of icons.
    size_t getIconCount() const
    { return icons_.size(); }

    /// ### method `nr::dune2::IconSet.getIcon`
    /// #### Parameters
    /// - `icon_index` - the tile index.
    /// #### Return
    /// `IconSet::Icon` - an icon _icons[icons_index]_.
    const Icon &getIcon(size_t icon_index) const
    { return icons_.at(icon_index); }

    /// ### method `nr::dune2::IconSet.begin`
    /// #### Return
    /// `IconSet::IconIterator` - an iterator on the first tile.
    IconIterator begin() const
    { return icons_.begin(); }

    /// ### method `nr::dune2::IconSet.end`
    /// #### Return
    /// `IconSet::IconIterator` - an iterator on the last tile.
    IconIterator end() const
    { return icons_.end(); }

public:
    template <typename T>
    void push_back(T &&icon) {
        icons_.push_back(std::forward<T>(icon));
    }

private:
    std::vector<Icon> icons_;
};
} // namespace nr::dune2

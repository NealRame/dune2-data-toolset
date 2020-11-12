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
    class Tile: public Surface {
        friend class ICN;

    public:
        struct Info {
            unsigned int width;
            unsigned int height;
            unsigned int bitPerPixels;

            std::size_t tileSize() const
            { return (width*height*bitPerPixels)/8; }

            std::size_t paletteSize() const
            { return 1<<bitPerPixels; }
        };

    public:
        virtual std::size_t getWidth() const override;
        virtual std::size_t getHeight() const override;
        virtual Color getPixel(std::size_t x, std::size_t y) const override;

    private:
        Tile(const Palette &, const Info &, const std::vector<uint8_t> &, const std::vector<uint8_t> &);

    private:
        const Palette &palette_;
        const Info &info_;
        const std::vector<uint8_t> &data_;
        const std::vector<uint8_t> &paletteIndexes_;
    };

    class Icon: public Surface {
        friend class ICN;
    
    public:
        virtual std::size_t getWidth() const override;
        virtual std::size_t getHeight() const override;
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

    template<typename T>
    struct Range {
        using iterator = T;

        iterator first, last;

        iterator begin() { return first; }
        iterator end() { return last; }
    };

using TileIterator = Iterator<Tile>;
using TileRange = Range<TileIterator>;

using IconIterator = Iterator<Icon>;
using IconRange = Range<IconIterator>;

public:
    ICN(const Palette &);

public:
    static std::optional<ICN> load(
        const std::string &map_filepath,
        const std::string &icn_filepath,
        const Palette &
    );

public:
    std::size_t tileCount() const;
    Tile getTile(std::size_t tile_index) const;

    TileIterator tiles_begin() const;
    TileIterator tiles_end() const;

    TileRange tiles() const
    { return TileRange{tiles_begin(), tiles_end()}; }

public:
    std::size_t iconCount() const;
    Icon getIcon(std::size_t icon_index) const;

    IconIterator icons_begin() const;
    IconIterator icons_end() const;

    IconRange icons() const
    { return IconRange{icons_begin(), icons_end()}; }

private:
    const Palette &palette_;
    Tile::Info tileInfo_;
    std::vector<std::vector<uint8_t>> tilesDataTable_;
    std::vector<uint8_t> tilesPaletteIndexesTable_;
    std::vector<std::vector<uint8_t>> tilesPaletteIndexes_;
    std::vector<std::vector<unsigned int>> iconsTilesMapping_;
};
} // namespace nr::dune2

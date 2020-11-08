#include "icn.hpp"
#include "io.hpp"

#include <fstream>
#include <iostream>

namespace nr::dune2 {

namespace {

using IMap = std::vector<std::vector<unsigned int>>;

IMap
read_icons_map(std::fstream &input) {
    std::vector<unsigned int> tile_indexes;
    while (!input.eof()) {
        tile_indexes.emplace_back(io::readLEInteger<2>(input));
    }

    const auto count = tile_indexes[0];

    IMap icons_map;
    std::transform(
        tile_indexes.begin() + 1,
        tile_indexes.begin() + count,
        tile_indexes.begin() + 2,
        std::back_inserter(icons_map),
        [&](auto first, auto last) {
            last = last > 0 ? last : tile_indexes.size();
            return std::vector(
                tile_indexes.begin() + first,
                tile_indexes.begin() + last
            );
        }
    );

    return icons_map;
}

using SSet = std::vector<std::vector<uint8_t>>;
using RPal = std::vector<std::vector<uint8_t>>;
using RTbl = std::vector<uint8_t>;

ICN::Info
read_sinf_chunk(std::fstream &input) {
    using namespace std::literals;

    io::check(input, []{ return "SINF"sv; });

    const auto sinf_chunk_size = io::readBEInteger<4>(input);

    const auto width  = io::readLEInteger<1>(input);
    const auto height = io::readLEInteger<1>(input);
    const auto shift  = io::readLEInteger<1>(input);

    const auto bit_per_pixel = io::readLEInteger<1>(input);

    return ICN::Info{
        width  << shift,
        height << shift,
        bit_per_pixel
    };
}

SSet
read_sset_chunk(std::fstream &input, const ICN::Info &info) {
    using namespace std::literals;

    io::check(input, []{ return "SSET"sv; });

    const auto sset_chunk_size = io::readBEInteger<4>(input);
    const auto tile_size = info.tileSize();
    const auto tile_count = (sset_chunk_size - 4)/tile_size;

    // We ignore the four next bytes.
    // They must contains 00 00 AB CD 00 00 00 00.
    // ABCD being little endian representation of sset_chunk_size - 4.
    input.ignore(8);

    SSet sset;
    std::generate_n(
        std::back_inserter(sset),
        tile_count,
        [&] { return io::readData<uint8_t>(input, tile_size); }
    );
    return sset;
}

RPal
read_rpal_chunk(std::fstream &input, const ICN::Info &info) {
    using namespace std::literals;

    io::check(input, []{ return "RPAL"sv; });

    const auto rpal_chunk_size = io::readBEInteger<4>(input);
    const auto pal_size = info.paletteSize();
    const auto pal_count = rpal_chunk_size/pal_size;

    RPal rpal;
    std::generate_n(
        std::back_inserter(rpal),
        pal_count,
        [&] { return io::readData<uint8_t>(input, pal_size); }
    );
    return rpal;
}

RTbl
read_rtbl_chunk(std::fstream &input, const ICN::Info &info) {
    using namespace std::literals;

    io::check(input, []{ return "RTBL"sv; });

    const auto rtbl_chunk_size = io::readBEInteger<4>(input);
    const auto rtbl = io::readData<uint8_t>(input, rtbl_chunk_size);

    return rtbl;
}

struct Tile: public Surface {
    virtual std::size_t getWidth() const override {
        return info.width;
    }

    virtual std::size_t getHeight() const override {
        return info.width;
    }

    virtual Color getPixel(std::size_t x, std::size_t y) const override {
        const auto index = y*getWidth() + x;

        const auto k = (index*info.bitPerPixels)/8;
        const auto s = info.bitPerPixels - (index*info.bitPerPixels)%8;
        const auto p = (data[k] >> s) & ((1 << info.bitPerPixels) - 1);

        return palette[paletteOffsets[p]];
    }

    Tile(
        const ICN::Info &info,
        const std::vector<uint8_t> &data,
        const std::vector<uint8_t> &paletteOffsets,
        const Palette &palette)
        : info{info}
        , data{data}
        , paletteOffsets{paletteOffsets}
        , palette{palette} {
    }

    const ICN::Info &info;
    const std::vector<uint8_t> &data;
    const std::vector<uint8_t> &paletteOffsets;
    const Palette &palette;
};

} // namespace

struct ICN::Icon::impl {
    const Palette &palette;
    const std::size_t width{1};
    const std::size_t height{1};
    const ICN::Info tileInfo;
    std::vector<std::vector<uint8_t>> tiles;
    std::vector<std::vector<uint8_t>> paletteOffsets;

    static std::tuple<size_t, size_t> shape(unsigned int index, size_t size) {
        std::tuple<size_t, size_t> wh;
        switch (index) {
        case 0:
        case 1:
        case 2:
        case 3:
            wh = std::make_tuple(3, 2);
            break;
        case 4:
            wh = std::make_tuple(4, 2);
            break;
        case 5:
            wh = std::make_tuple(15, 5);
            break;
        case 6:
            wh = std::make_tuple(4, 4);
            break;
        case 8:
            wh = std::make_tuple(27, 3);
            break;
        case 10:
            wh = std::make_tuple(3, 12);
            break;
        case 11:
            wh = std::make_tuple(2, 12);
            break;
        case 12:
        case 13:
            wh = std::make_tuple(3, 16);
            break;
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
            wh = std::make_tuple(2, 8);
            break;
        case 19:
            wh = std::make_tuple(3, 30);
            break;
        case 20:
        case 21:
            wh = std::make_tuple(3, 20);
            break;
        case 22:
        case 23:
            wh = std::make_tuple(1, 10);
            break;
        case 24:
            wh = std::make_tuple(2, 8);
            break;
        case 25:
            wh = std::make_tuple(2, 12);
            break;
        default:
            wh = std::make_tuple(size, 1);
            break;
        }

        std::cout
            << "ICON#" << index
            << " size=" << size
            << " w=" << std::get<0>(wh)
            << " h=" << std::get<1>(wh)
        << std::endl;

        assert(std::get<0>(wh)*std::get<1>(wh) <= size);

        return wh;
    }

    impl(const Palette &palette, std::size_t width, std::size_t height, const ICN::Info &tileInfo)
        : palette{palette}
        , width{width}
        , height{height}
        , tileInfo{tileInfo} {
    }
};

ICN::Icon::Icon(const Palette &palette, size_t width, size_t height, const ICN::Info &info)
    : d{std::make_unique<impl>(palette, width, height, info)} {
}

ICN::Icon::Icon(Icon &&rhs) {
    *this = std::move(rhs);
}

ICN::Icon &
ICN::Icon::operator=(Icon &&rhs) {
    d = std::move(rhs.d);
    return *this;
}

ICN::Icon::~Icon() {
}

std::size_t
ICN::Icon::getWidth() const {
    return d->tileInfo.width*d->width;
}

std::size_t
ICN::Icon::getHeight() const {
    return d->tileInfo.height*d->height;
}

Color
ICN::Icon::getPixel(std::size_t x, std::size_t y) const {
    assert(x < getWidth());
    assert(y < getHeight());

    const auto tile_index = x/d->tileInfo.width + y/d->tileInfo.height*d->width;
    const auto tile = Tile(
        d->tileInfo,
        d->tiles[tile_index],
        d->paletteOffsets[tile_index],
        d->palette
    );

    x = x%d->tileInfo.width;
    y = y%d->tileInfo.height;

    return tile.getPixel(x, y);
}

std::optional<ICN>
ICN::load(
    const std::string &map_filepath,
    const std::string &icn_filepath,
    const Palette &palette) {
    using namespace std::literals;
    std::fstream map_input(map_filepath, std::ios::binary|std::ios::in);
    std::fstream icn_input(icn_filepath, std::ios::binary|std::ios::in);

    auto icn = std::make_optional<ICN>();

    try {
        const auto imap = read_icons_map(map_input);

        // First read IFF chunk group ID (wich must be FORM)
        io::check(icn_input, []{ return "FORM"sv;});

        const auto chunk_group_size = io::readBEInteger<4>(icn_input);

        // Check group type is ICON
        io::check(icn_input, []{ return "ICON"sv;});

        auto info = read_sinf_chunk(icn_input);
        auto sset = read_sset_chunk(icn_input, info);
        auto rpal = read_rpal_chunk(icn_input, info);
        auto rtbl = read_rtbl_chunk(icn_input, info);

        assert(sset.size() == rtbl.size());

        std::transform(
            imap.begin(), imap.end(),
            std::back_inserter(icn->icons_),
            [&, i = 0](const auto &tile_indexes) mutable {
                const auto [icon_width, icon_height] = Icon::impl::shape(i++, tile_indexes.size());

                Icon icon(palette, icon_width, icon_height, info);

                for (const auto tile_index: tile_indexes) {
                    assert(tile_index < sset.size());
                    assert(tile_index < rtbl.size());
                    icon.d->tiles.push_back(sset[tile_index]);
                    icon.d->paletteOffsets.push_back(rpal[rtbl[tile_index]]);
                }

                return std::move(icon);
            }
        );
    } catch (...) {
        return std::nullopt;
    }

    return icn;
}

ICN::const_iterator
ICN::begin() const {
    return icons_.begin();
}

ICN::const_iterator
ICN::end() const {
    return icons_.end();
}

ICN::const_iterator
ICN::cbegin() const {
    return begin();
}

ICN::const_iterator
ICN::cend() const {
    return end();
}
} // namespace nr::dune2

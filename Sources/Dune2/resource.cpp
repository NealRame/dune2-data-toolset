#include "resource.hpp"

#include <Dune2RC/rc.pb.h>

#include <fstream>
#include <limits>

namespace nr::dune2 {

struct Resource::impl {
    nr::dune2::rc::Data rc;
};

Resource::Resource()
    : d{std::make_unique<impl>()} {
}

Resource::~Resource() {
}

void
Resource::addPalette(const Palette &palette) {
    nr::dune2::rc::Data::Palette pb_palette;

    constexpr auto scale = static_cast<float>(Palette::Color::ChannelMax);

    for (auto &&color: palette) {
        auto color_pb = pb_palette.add_colors();

        color_pb->set_alpha(1.);
        color_pb->set_red(color.red/scale);
        color_pb->set_green(color.green/scale);
        color_pb->set_blue(color.blue/scale);
    }

    const auto &name = palette.getName();
    auto &pb_palettes = *(d->rc.mutable_palettes());

    if (pb_palettes.contains(name)) {
        pb_palettes.erase(name);
    }

    pb_palettes[name] = pb_palette;
}

std::optional<Palette>
Resource::getPalette(const std::string &name) const {
    if (d->rc.palettes().contains(name)) {
        std::optional<Palette> palette = std::make_optional<Palette>(name);

        constexpr auto scale = Palette::Color::ChannelMax;

        const auto pb_palette = d->rc.palettes().at(name);
        for (auto i = 0; i < pb_palette.colors_size(); ++i) {
            const auto pb_color = pb_palette.colors(i);
            palette->at(i) = Palette::Color{
                .red   = static_cast<uint8_t>(pb_color.red()*scale),
                .green = static_cast<uint8_t>(pb_color.green()*scale),
                .blue  = static_cast<uint8_t>(pb_color.blue()*scale)
            };
        }

        return palette;
    }
    return std::nullopt;
}

std::vector<std::string>
Resource::getPaletteList() const {
    std::vector<std::string> palettes;
    for (auto &&item: d->rc.palettes()) {
        palettes.push_back(item.first);
    }
    return palettes;
}

void
Resource::addTileset(const Tileset &tileset) {
    nr::dune2::rc::Data::Tileset pb_tileset;

    for (auto &&tile: tileset) {
        auto pb_tile = pb_tileset.add_tiles();

        pb_tile->set_width(tile.getWidth());
        pb_tile->set_height(tile.getHeight());
        pb_tile->set_pixels(tile.getData());
        if (tile.hasRemapTable()) {
            pb_tile->set_remaptable(tile.getRemapTableData());
        }
    }

    const auto &name = tileset.getName();
    auto &pb_tilesets = *(d->rc.mutable_tilesets());

    if (pb_tilesets.contains(name)) {
        pb_tilesets.erase(name);
    }

    pb_tilesets[name] = pb_tileset;
}

std::optional<Tileset>
Resource::getTileset(const std::string &name) const {
    if (d->rc.tilesets().contains(name)) {
        std::optional<Tileset> tileset = std::make_optional<Tileset>(name);

        const auto pb_tileset = d->rc.tilesets().at(name);
        for (auto i = 0; i < pb_tileset.tiles_size(); ++i) {
            const auto pb_tile = pb_tileset.tiles(i);
            tileset->push_back(Tileset::Tile(
                pb_tile.width(),
                pb_tile.height(),
                pb_tile.pixels(),
                pb_tile.remaptable()
            ));
        }
        return tileset;
    }
    return std::nullopt;
}

std::vector<std::string>
Resource::getTilesetList() const {
    std::vector<std::string> tilesets;
    for (auto &&item: d->rc.tilesets()) {
        tilesets.push_back(item.first);
    }
    return tilesets;
}

void
Resource::deserialize(const std::filesystem::path &filepath) {
    std::ifstream input;

    input.open(filepath, std::ios::binary);
    if (!d->rc.ParseFromIstream(&input)) {
        throw deserialization_failure();
    }
}

void
Resource::serialize(const std::filesystem::path &filepath) const {
    std::ofstream output;

    output.open(filepath, std::ios::binary);
    if (!d->rc.SerializeToOstream(&output)) {
        throw serialization_failure();
    }
}

} // namespace nr::dune2

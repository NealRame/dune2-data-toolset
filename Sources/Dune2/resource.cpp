#include "resource.hpp"
#include "io.hpp"

#include <Dune2RC/rc.pb.h>

#include <fmt/format.h>

#include <fstream>
#include <iostream>
#include <regex>

namespace nr::dune2 {
namespace fs = std::filesystem;

namespace {
bool
filepath_match(const fs::path &filepath, const std::string extension) {
    const auto ext = filepath.extension().string();
    return std::regex_match(ext, std::regex(
        fmt::format(R"(^\{}$)", extension),
        std::regex_constants::icase
    ));
}
} // namespace

struct Resource::impl {
    nr::dune2::rc::Data rc;
};

Resource::Resource()
    : d{std::make_unique<impl>()} {
}

Resource::~Resource() {
}

bool
Resource::hasPalette(const std::string &name) const {
    return d->rc.palettes().contains(name);
}

void
Resource::removePalette(const std::string &name) {
    d->rc.mutable_palettes()->erase(name);
}

void
Resource::importPalette(
    const std::string &name,
    const std::filesystem::path &filepath) {
    constexpr auto scale = static_cast<float>(Palette::Color::ChannelMax);

    if (hasPalette(name)) {
        throw PaletteAlreadyExistError();
    }

    nr::dune2::rc::Data::Palette pb_palette;
    nr::dune2::Palette palette;

    palette.load(filepath);

    for (auto &&color: palette) {
        auto color_pb = pb_palette.add_colors();

        color_pb->set_alpha(1.);
        color_pb->set_red(color.red/scale);
        color_pb->set_green(color.green/scale);
        color_pb->set_blue(color.blue/scale);
    }

    (*d->rc.mutable_palettes())[name] = pb_palette;
}

Palette
Resource::getPalette(const std::string &name) const {
    if (d->rc.palettes().contains(name)) {
        throw ResourceNotFound(name);
    }

    constexpr auto scale = Palette::Color::ChannelMax;
    const auto pb_palette = d->rc.palettes().at(name);
    Palette palette;

    for (auto i = 0; i < pb_palette.colors_size(); ++i) {
        const auto pb_color = pb_palette.colors(i);
        palette.at(i) = Palette::Color{
            .red   = static_cast<uint8_t>(pb_color.red()*scale),
            .green = static_cast<uint8_t>(pb_color.green()*scale),
            .blue  = static_cast<uint8_t>(pb_color.blue()*scale)
        };
    }

    return palette;
}

std::vector<std::string>
Resource::getPaletteList() const {
    std::vector<std::string> palettes;
    for (auto &&item: d->rc.palettes()) {
        palettes.push_back(item.first);
    }
    return palettes;
}

bool
Resource::hasTileset(const std::string &name) const {
    return d->rc.tilesets().contains(name);
}

void
Resource::createTileset(const std::string &name) {
    if (!d->rc.tilesets().contains(name)) {
        auto &tileset = *d->rc.mutable_tilesets();
        tileset[name] = nr::dune2::rc::Data::Tileset();
    }
}

void
Resource::removeTileset(const std::string &name) {
    d->rc.mutable_tilesets()->erase(name);
}

void
Resource::importTileset(
    const std::string &name,
    const std::filesystem::path &filepath) {
    if (!hasTileset(name)) {
        throw ResourceNotFound(name);
    }

    auto &pb_tileset = d->rc.mutable_tilesets()->at(name);
    nr::dune2::Tileset tileset;

    if (filepath_match(filepath, ".icn")) {
        tileset.loadFromICN(filepath);
    }

    if (filepath_match(filepath, ".shp")) {
        tileset.loadFromSHP(filepath);
    }

    for (auto &&tile: tileset) {
        auto pb_tile = pb_tileset.add_tiles();

        pb_tile->set_width(tile.getWidth());
        pb_tile->set_height(tile.getHeight());
        pb_tile->set_pixels(tile.getData());
        if (tile.hasRemapTable()) {
            pb_tile->set_remaptable(tile.getRemapTableData());
        }
    }
}

Tileset
Resource::getTileset(const std::string &name) const {
    if (!d->rc.tilesets().contains(name)) {
        throw ResourceNotFound(name);
    }
    
    const auto pb_tileset = d->rc.tilesets().at(name);
    Tileset tileset;

    for (auto i = 0; i < pb_tileset.tiles_size(); ++i) {
        const auto pb_tile = pb_tileset.tiles(i);
        tileset.push_back(Tileset::Tile(
            pb_tile.width(),
            pb_tile.height(),
            pb_tile.pixels(),
            pb_tile.remaptable()
        ));
    }

    return tileset;
}

std::vector<std::string>
Resource::getTilesetList() const {
    std::vector<std::string> tilesets;
    for (auto &&item: d->rc.tilesets()) {
        tilesets.push_back(item.first);
    }
    return tilesets;
}

std::vector<std::string>
Resource::getSoundList(const std::string &name) const {
    std::vector<std::string> sounds;
    if (hasSoundset(name)) {
        const auto &soundset = d->rc.soundsets().at(name);
        for (auto i = 0; i < soundset.sounds_size(); ++i) {
            const auto sound = soundset.sounds(i);
            sounds.push_back(sound.name());
        }
    }
    return sounds;
}

bool
Resource::hasSoundset(const std::string &name) const {
    return d->rc.soundsets().contains(name);
}

void
Resource::createSoundset(const std::string &name) {
    if (!d->rc.soundsets().contains(name)) {
        auto &soundsets = *d->rc.mutable_soundsets();
        soundsets[name] = nr::dune2::rc::Data::Soundset();
    }
}

void
Resource::addSound(const std::string &soundset_name, const fs::path &filepath) {
    if (auto input = std::ifstream(filepath, std::ifstream::binary)) {
        auto &soundset = d->rc.mutable_soundsets()->at(soundset_name);
        auto sound = soundset.add_sounds();
        sound->set_name(filepath.stem());
        sound->set_data(io::readString(input, fs::file_size(filepath)));
    }
}

void
Resource::removeSoundset(const std::string &name) {
    d->rc.mutable_soundsets()->erase(name);
}

std::vector<std::string>
Resource::getSoundsetList() const {
    std::vector<std::string> soundsets;
    for (auto &&item: d->rc.soundsets()) {
        soundsets.push_back(item.first);
    }
    return soundsets;
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

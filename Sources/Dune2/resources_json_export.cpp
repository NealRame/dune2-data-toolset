#include "resources.hpp"

#include <cppcodec/base64_rfc4648.hpp>

#include <rapidjson/document.h>
// #include <rapidjson/prettywriter.h>
// #include <rapidjson/stringbuffer.h>
// #include <rapidjson/ostreamwrapper.h>

#include <fstream>
#include <iostream>

namespace nr::dune2 {

using rapidjson::Document;
using rapidjson::Value;
using rapidjson::MemoryPoolAllocator;

Value export_palette(
    MemoryPoolAllocator<> &allocator,
    const Palette &palette
) {
    Value value(rapidjson::kArrayType);
    for (auto &&color: palette) {
        value.PushBack(
            Value(rapidjson::kArrayType)
                .PushBack(Value(color.red), allocator)
                .PushBack(Value(color.green), allocator)
                .PushBack(Value(color.blue), allocator),
            allocator
        );
    }
    return value;
}

Value export_tile(
    MemoryPoolAllocator<> &allocator,
    const Tileset::Tile &tile
) {
    using base64 = cppcodec::base64_rfc4648;
    Value value(rapidjson::kObjectType);

    value.AddMember("w", Value((unsigned int)tile.getWidth()), allocator);
    value.AddMember("h", Value((unsigned int)tile.getHeight()), allocator);

    const auto data = base64::encode(tile.getData());
    value.AddMember(
        "data",
        Value().SetString(data.c_str(), allocator),
        allocator
    );

    if (tile.hasRemapTable()) {
        const auto remap = base64::encode(tile.getRemapTableData());
        value.AddMember(
            "remap",
            Value().SetString(remap.c_str(), allocator),
            allocator
        );
    }

    return value;
}

Value export_tileset(
    MemoryPoolAllocator<> &allocator,
    const Tileset &tileset
) {
    Value tiles(rapidjson::kArrayType);
    for (const auto &tile: tileset) {
        tiles.PushBack(export_tile(allocator, tile), allocator);
    }
    return tiles;
}

Value export_soundset(
    MemoryPoolAllocator<> &allocator,
    const std::vector<std::pair<std::string, std::string>> &sounds
) {
    using base64 = cppcodec::base64_rfc4648;
    constexpr auto Name = 0u;
    constexpr auto Data = 1u;

    Value soundmap(rapidjson::kObjectType);
    for (auto &&sound: sounds) {
        const auto name = std::get<Name>(sound);
        const auto data = base64::encode(std::get<Data>(sound));
        soundmap.AddMember(
            Value().SetString(name.c_str(), allocator),
            Value().SetString(data.c_str(), allocator),
            allocator
        );
    }
    return soundmap;
}

rapidjson::Document
Resource::json_export() const {
    rapidjson::Document d;
    auto &allocator = d.GetAllocator();

    d.SetObject();

    // export palette
    if (hasPalette()) {
        auto palette = getPalette();
        d.AddMember("palette", export_palette(allocator, palette), allocator);
    }

    // export tilesets
    Value tilesets(rapidjson::kObjectType);
    for (auto &&name: getTilesetList()) {
        tilesets.AddMember(
            Value().SetString(name.c_str(), allocator),
            export_tileset(allocator, getTileset(name)),
            allocator
        );
    }
    d.AddMember("tilesets", tilesets, allocator);

    // export soundsets
    Value soundsets(rapidjson::kObjectType);
    for (auto &&soundset: getSoundsetList()) {
        soundsets.AddMember(
            Value().SetString(soundset.c_str(), allocator),
            export_soundset(allocator, getAllSounds(soundset)),
            allocator
        );
    }
    d.AddMember("soundsets", soundsets, allocator);

    return d;
}

} // namespace nr::dune2

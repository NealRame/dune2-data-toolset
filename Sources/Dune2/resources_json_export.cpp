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

    Value data;
    data.SetString(base64::encode(tile.getData()).c_str(), allocator);
    value.AddMember("data", data, allocator);

    if (tile.hasRemapTable()) {
        Value remap;
        remap.SetString(base64::encode(tile.getRemapTableData()).c_str(), allocator);
        value.AddMember("remap", remap, allocator);
    }

    return value;
}

Value export_tileset(
    MemoryPoolAllocator<> &allocator,
    const Tileset &tileset
) {
    Value value(rapidjson::kArrayType);
    for (const auto &tile: tileset) {
        value.PushBack(export_tile(allocator, tile), allocator);
    }
    return value;
}

rapidjson::Document
Resource::json_export() const {
    rapidjson::Document d;
    auto &allocator = d.GetAllocator();

    d.SetObject();

    // export palette.json
    Value palettes(rapidjson::kObjectType);
    for (const auto &name: getPaletteList()) {
        Value key;
        key.SetString(name.c_str(), allocator);
        palettes.AddMember(key, export_palette(allocator, getPalette(name)), allocator);
    }
    d.AddMember("palettes", palettes, allocator);

    // export tilesets
    Value tilesets(rapidjson::kObjectType);
    for (const auto &name: getTilesetList()) {
        Value key;
        key.SetString(name.c_str(), allocator);
        tilesets.AddMember(key, export_tileset(allocator, getTileset(name)), allocator);
    }
    d.AddMember("tilesets", tilesets, allocator);

    return d;
}

} // namespace nr::dune2

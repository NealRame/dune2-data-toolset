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
        d.AddMember("palette", palette.toJSON(d), allocator);
    }

    // export tilesets
    Value tilesets(rapidjson::kObjectType);
    for (auto &&name: getTilesetList()) {
        tilesets.AddMember(
            Value().SetString(name.c_str(), allocator),
            getTileset(name).toJSON(d),
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

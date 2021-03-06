#include "image_set.hpp"

#include <cppcodec/base64_rfc4648.hpp>

namespace nr::dune2 {

using rapidjson::Document;
using rapidjson::Value;
using rapidjson::MemoryPoolAllocator;

namespace {
Value tile_to_JSON(
    MemoryPoolAllocator<> &allocator,
    const Image &tile
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

}

rapidjson::Document
ImageSet::toJSON() const {
    rapidjson::Document doc;
    auto &allocator = doc.GetAllocator();
    auto &tiles = doc.SetArray();
    for (const auto &tile: tiles_) {
        tiles.PushBack(tile_to_JSON(allocator, tile), allocator);
    }
    return doc;
}
} // namespace nr::dune2

#include "tileset.hpp"

#include <cppcodec/base64_rfc4648.hpp>

namespace nr::dune2 {

std::string
decode(const rapidjson::Value &value) {
    using base64 = cppcodec::base64_rfc4648;
    return base64::decode<std::string>(std::string(value.GetString()));
}

namespace {
Tileset::Tile load_tile_from_JSON(const rapidjson::Value &value) {
    unsigned int width{value.FindMember("w")->value.GetUint()};
    unsigned int height{value.FindMember("h")->value.GetUint()};
    std::string data(decode(value.FindMember("data")->value));

    if (value.HasMember("remap")) {
        std::string remap(decode(value.FindMember("remap")->value));
        return Tileset::Tile(width, height, data, remap);
    }
    return Tileset::Tile(width, height, data);
}
}

void
Tileset::loadFromJSON(const rapidjson::Value &value) {
    tiles_.clear();
    std::transform(
        value.Begin(),
        value.End(),
        std::back_inserter(tiles_),
        load_tile_from_JSON
    );
}
} // namespace nr::dune2

#include "image_set.hpp"
#include "io.hpp"

#include <cppcodec/base64_rfc4648.hpp>

#include <fstream>

namespace nr::dune2 {

std::string
decode(const rapidjson::Value &value) {
    using base64 = cppcodec::base64_rfc4648;
    return base64::decode<std::string>(std::string(value.GetString()));
}

namespace {
ImageSet::Image load_tile_from_JSON(const rapidjson::Value &value) {
    unsigned int width{value.FindMember("w")->value.GetUint()};
    unsigned int height{value.FindMember("h")->value.GetUint()};
    std::string data(decode(value.FindMember("data")->value));

    if (value.HasMember("remap")) {
        std::string remap(decode(value.FindMember("remap")->value));
        return ImageSet::Image(width, height, data, remap);
    }
    return ImageSet::Image(width, height, data);
}
}

void
ImageSet::loadFromJSON(const std::filesystem::path &filepath) {
    std::ifstream input(filepath);
    const auto json = io::loadJSON(input);
    std::transform(
        json.Begin(),
        json.End(),
        std::back_inserter(tiles_),
        load_tile_from_JSON
    );
}
} // namespace nr::dune2

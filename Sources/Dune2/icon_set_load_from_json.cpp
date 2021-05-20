#include "icon_set.hpp"
#include "io.hpp"

#include <fstream>

namespace nr::dune2 {

namespace {
IconSet::Icon load_icon_from_JSON(const rapidjson::Value &value) {
    const auto &images_indexes_value = value.FindMember("images")->value;

    IconSet::Icon::TileIndexList image_indexes;
    std::transform(
        images_indexes_value.Begin(),
        images_indexes_value.End(),
        std::back_inserter(image_indexes),
        [](const auto &index_value) {
            return index_value.GetInt();
        }
    );

    return IconSet::Icon(
        value.FindMember("columns")->value.GetUint(),
        value.FindMember("rows")->value.GetUint(),
        std::move(image_indexes)
    );
}
} // namespace

void
IconSet::loadFromJSON(const std::filesystem::path &json_path) {
    std::fstream input(json_path);
    const auto json = io::loadJSON(input);

    std::transform(
        json.Begin(),
        json.End(),
        std::back_inserter(icons_),
        load_icon_from_JSON
    );
}

} // namespace nr::dune2

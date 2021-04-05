#include "palette.hpp"
#include "io.hpp"

#include <fstream>

namespace fs = std::filesystem;
namespace nr::dune2 {

Palette::Palette()
    : colors_(256) {
}

void
Palette::loadFromPAL(const fs::path &filepath) {
    std::ifstream input;

    input.exceptions(std::ifstream::failbit);
    input.open(filepath, std::ios::binary);

    std::generate(
        colors_.begin(),
        colors_.end(),
        [&] {
            using nr::dune2::io::readLEInteger;
            Color color;

            color.red   = readLEInteger<1, uint8_t>(input)*4;
            color.green = readLEInteger<1, uint8_t>(input)*4;
            color.blue  = readLEInteger<1, uint8_t>(input)*4;

            return color;
        }
    );
}

void
Palette::loadFromJSON(const rapidjson::Value &json) {
    std::transform(
        json.Begin(),
        json.End(),
        colors_.begin(),
        [](const auto &v) {
            return Palette::Color{
                static_cast<Palette::Color::Channel>(v[0].GetInt()),
                static_cast<Palette::Color::Channel>(v[1].GetInt()),
                static_cast<Palette::Color::Channel>(v[2].GetInt()),
            };
        }
    );
}

rapidjson::Value
Palette::toJSON(rapidjson::Document &doc) const {
    using rapidjson::Value;

    Value value(rapidjson::kArrayType);
    auto &allocator = doc.GetAllocator();

    for (auto &&color: colors_) {
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

} // namespace nr::dune2

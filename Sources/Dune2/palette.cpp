#include "palette.hpp"
#include "bmp.hpp"
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
Palette::loadFromJSON(const std::filesystem::path &filepath) {
    std::ifstream input(filepath);
    const auto json = io::loadJSON(input);
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

rapidjson::Document
Palette::toJSON() const {
    using rapidjson::Value;

    rapidjson::Document doc;

    auto &allocator = doc.GetAllocator();
    auto &value = doc.SetArray();

    for (auto &&color: colors_) {
        value.PushBack(
            Value(rapidjson::kArrayType)
                .PushBack(Value(color.red), allocator)
                .PushBack(Value(color.green), allocator)
                .PushBack(Value(color.blue), allocator),
            allocator
        );
    }
    return doc;
}

BMP 
Palette::toBMP() const {
    const auto color_per_row = 8;
    const auto color_size = 32;
    const auto width = color_size*color_per_row;
    const auto height = color_size*colors_.size()/color_per_row;

    BMP bmp(width, height);
    for (auto i = 0; i < colors_.size(); ++i) {
        const auto row = i/color_per_row;
        const auto col = i%color_per_row;
        bmp.fillRect(col*color_size, row*color_size, color_size, color_size, colors_.at(i));
    }
    return bmp;
}

} // namespace nr::dune2

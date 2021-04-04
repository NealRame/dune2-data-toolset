#include "palette.hpp"
#include "io.hpp"

#include <rapidjson/document.h>

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
Palette::loadFromJSON(std::string_view json) {
    rapidjson::Document doc;
    doc.Parse(json.data());

    std::transform(
        doc.Begin(),
        doc.End(),
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


} // namespace nr::dune2

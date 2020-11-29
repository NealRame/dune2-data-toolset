#include "palette.hpp"

#include <Dune2/io.hpp>

#include <fstream>

namespace nr::dune2 {

Palette::Palette(const std::string &name)
    : colors_(256)
    , name_{name} {
}

const std::string &
Palette::getName() const {
    return name_;
}

void
Palette::load(const std::string &filepath) {
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

} // namespace nr::dune2

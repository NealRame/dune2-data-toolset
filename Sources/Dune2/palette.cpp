#include "palette.hpp"

#include <Dune2/io.hpp>

#include <fstream>

namespace std {

std::istream &
operator>>(std::istream &input, nr::dune2::Palette::Color &color) {
    using nr::dune2::io::readLEInteger;
    if (input) {
        color.red = readLEInteger<1, uint8_t>(input)*4;
        color.green = readLEInteger<1, uint8_t>(input)*4;
        color.blue = readLEInteger<1, uint8_t>(input)*4;
    }
    return input;
}

} // namespace std


namespace nr::dune2 {

Palette::Palette()
    : colors_(256) {
}

std::optional<Palette>
Palette::load(const std::string &filepath) {
    std::fstream input(filepath, std::ios::binary|std::ios::in);

    auto palette = std::make_optional<Palette>();

    std::copy(
        std::istream_iterator<Color>(input),
        std::istream_iterator<Color>(),
        palette->colors_.begin()
    );

    return palette;
}

} // namespace nr::dune2

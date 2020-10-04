#include "palette.hpp"

#include <Dune2/io.hpp>

#include <fstream>

namespace std {

std::istream &
operator>>(std::istream &input, nr::dune2::Palette::Color &color) {
    using nr::dune2::io::readInteger;
    if (input) {
        color.red = readInteger<1, uint8_t>(input);
        color.green = readInteger<1, uint8_t>(input);
        color.blue = readInteger<1, uint8_t>(input);
    }
    return input;
}

} // namespace std


namespace nr::dune2 {

std::optional<Palette>
Palette::load(const std::string &filepath) {
    std::fstream input(filepath, std::ios::binary|std::ios::in);

    auto palette = std::make_optional<Palette>();

    std::copy(
        std::istream_iterator<Palette::Color>(input),
        std::istream_iterator<Palette::Color>(),
        std::back_inserter(palette->colors_)
    );

    return palette;
}

} // namespace nr::dune2

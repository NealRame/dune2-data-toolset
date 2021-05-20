#include "image.hpp"
#include "io.hpp"

#include <fstream>

namespace nr::dune2 {
namespace fs = std::filesystem;

const auto cps_image_data_size = 64000U;

void
Image::loadFromCPS(const fs::path &cps_path) {
    std::ifstream input;

    input.open(cps_path, std::ios::binary);
    input.exceptions(std::ios::failbit);

    const auto file_size = io::readLEInteger<2>(input);
    const auto compression_type = io::readLEInteger<2>(input);
    const auto inflated_size = io::readLEInteger<4>(input);
    const auto palette_size = io::readLEInteger<2>(input);

    if (compression_type != 0 && compression_type != 4) {
        throw std::invalid_argument("corrupted file");
    }

    if (inflated_size != cps_image_data_size) {
        throw std::invalid_argument("corrupted file");
    }

    if (palette_size != 0) {
        throw std::invalid_argument("corrupted file");
    }

    const auto data = io::readLCWData(input, file_size - 8, cps_image_data_size);

    width_ = 320;
    height_ = 200;
    data_ = std::string(data.begin(), data.end());
}

} // namespace nr::dune2

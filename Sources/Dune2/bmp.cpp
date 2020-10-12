#include "bmp.hpp"
#include "io.hpp"

#include <cmath>
#include <fstream>

namespace std {
std::ostream &
operator<<(std::ostream &output, const nr::dune2::Color &c) {
    return output.put(c.red).put(c.green).put(c.blue);
}
} // namespace std


namespace nr::dune2 {

namespace {
constexpr long long unsigned
operator"" _ppi(long long unsigned res) {
    return 1000*res*.03937;
}

constexpr long long unsigned
operator"" _ppm(long long unsigned res) {
    return res;
}
} // namespace

BMP::BMP(unsigned int width, unsigned int height)
    : width_{width}
    , height_{height}
    , pixels_(width_*height_, Color{0, 0, 0}) {
}

void
BMP::putPixel(unsigned int x, unsigned y, const Color &c) {
    pixels_[y*width_ + x] = c;
}

void
BMP::fillRect(
    unsigned int x, unsigned y,
    unsigned int w, unsigned int h,
    const Color &c) {
    const auto x_max = std::min(x + w, width_), x_min = x;
    const auto y_max = std::min(y + h, height_);
    for (; y < y_max; ++y) {
        for (x = x_min; x < x_max; ++x) {
            putPixel(x, y, c);
        }
    }
}

void
BMP::store(const std::string &filepath) const {
    std::fstream output(filepath, std::ofstream::binary|std::ofstream::out);

    // Bitmap file header
    output.write("BM", 2);                // BMP signature

    const auto header_file_size_offset = output.tellp();
    io::writeInteger<4>(output, 0u);       // file size

    io::writeInteger<4>(output, 0u);      // reserved 1, reserved 2

    const auto header_pixels_offset = output.tellp();
    io::writeInteger<4>(output, 0u);      // offset to pixels array

    // Bitmap information
    io::writeInteger<4>(output, 40u);     // size of the info header (=40)
    io::writeInteger<4>(output, width_);  // width
    io::writeInteger<4>(output, height_); // height
    io::writeInteger<2>(output, 1u);      // count of color plan (=1)
    io::writeInteger<2>(output, 24u);     // number of bits per pixel (=24)
    io::writeInteger<4>(output, 0u);      // compression method (=0)
    io::writeInteger<4>(output, 0u);      // image size
    io::writeInteger<4>(output, 300_ppi); // horizontal resolution of the image
    io::writeInteger<4>(output, 300_ppi); // vertical resolution of the image
    io::writeInteger<4>(output, 0u);      // number of colors in the palette
    io::writeInteger<4>(output, 0u);      // number of important colors used

    // write pixels array offset
    {
        const auto pixels_array_offset = std::streamoff(output.tellp());
        io::OPosOffsetGuard _(output);
        io::writeInteger<4>(output.seekp(header_pixels_offset), pixels_array_offset);
    }

    // write pixels
    const auto row_size = static_cast<unsigned int>(std::ceilf(3.*width_/4.)*4);
    for (auto row = height_; row > 0; --row) {
        // Write row pixels
        std::copy_n(
            pixels_.begin() + (row - 1)*width_,
            width_,
            std::ostream_iterator<Color>(output)
        );
        // Write row padding
        std::fill_n(
            std::ostream_iterator<char>(output),
            row_size - 3*width_,
            0
        );
    }

    // write file size in bitmap file header
    {
        const auto file_size = std::streamoff(output.tellp());
        io::OPosOffsetGuard _(output);
        io::writeInteger<4>(output.seekp(header_file_size_offset), file_size);
    }

}

} // namespace nr::dune2

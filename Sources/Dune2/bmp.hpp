#pragma once

#include <Dune2/palette.hpp>
#include <Dune2/surface.hpp>

#include <filesystem>
#include <cstdint>
#include <vector>

namespace nr::dune2 {

class BMP {
public:
    BMP(size_t width, size_t height);

public:
    size_t width() const { return width_; }
    size_t height() const { return height_; }

public:
    void putPixel(size_t x, size_t y, const Palette::Color &);
    void fillRect(size_t x, size_t y, size_t w, size_t h, const Palette::Color &);
    void drawSurface(size_t x, size_t y, const Surface &, const Palette &);

public:
    void store(const std::filesystem::path &) const;

private:
    size_t width_;
    size_t height_;
    std::vector<Palette::Color> pixels_;
};

} // namespace nr::dune2

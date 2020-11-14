#pragma once

#include <Dune2/palette.hpp>
#include <Dune2/surface.hpp>

#include <cstdint>
#include <vector>

namespace nr::dune2 {

class BMP {
public:
    BMP(std::size_t width, std::size_t height);

public:
    std::size_t width() const  { return width_; }
    std::size_t height() const { return height_; }

public:
    void putPixel(std::size_t x, std::size_t y, const Palette::Color &);
    void fillRect(std::size_t x, std::size_t y, std::size_t w, std::size_t h, const Palette::Color &);
    void drawSurface(std::size_t x, std::size_t y, const Surface &, const Palette &);

public:
    void store(const std::string &filepath) const;

private:
    std::size_t width_;
    std::size_t height_;
    std::vector<Palette::Color> pixels_;
};

} // namespace nr::dune2

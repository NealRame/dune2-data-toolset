#pragma once

#include <Dune2/color.hpp>

#include <cstdint>
#include <vector>

namespace nr::dune2 {

class BMP {
public:
    BMP(unsigned int width, unsigned int height);

public:
    unsigned int width() const  { return width_; }
    unsigned int height() const { return height_; }

public:
    void putPixel(unsigned int x, unsigned y, const Color &);
    void fillRect(unsigned int x, unsigned y, unsigned int w, unsigned int h, const Color &);

public:
    void store(const std::string &filepath) const;

private:
    unsigned int width_;
    unsigned int height_;
    std::vector<Color> pixels_;
};

} // namespace nr::dune2

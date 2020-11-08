#pragma once

#include <Dune2/color.hpp>

#include <cstddef>

namespace nr::dune2 {
struct Surface {
    virtual std::size_t getWidth() const = 0;
    virtual std::size_t getHeight() const = 0;
    virtual Color getPixel(std::size_t x, std::size_t y) const = 0;
};
} // namespace nr::dune2

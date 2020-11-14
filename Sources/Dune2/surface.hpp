#pragma once

#include <cstddef>

namespace nr::dune2 {
struct Surface {
    /// ### method nr::dune2::Surface.getWidth
    /// #### Return
    /// `size_t` - this Surface width in pixels.
    virtual std::size_t getWidth() const = 0;

    /// ### method nr::dune2::Surface.getHeight
    /// #### Return
    /// `size_t` - this Surface height in pixels.
    virtual std::size_t getHeight() const = 0;

    /// ### method nr::dune2::Surface.getPixel
    /// Return the pixel value from given coordinates.
    /// #### Parameters
    /// * `size_t x`- the horizontal coordinate.
    /// * `size_t y`- the vertical coordinate.
    /// #### Return
    /// `size_t` - a color index for a 256 colors Palette.
    virtual std::size_t getPixel(std::size_t x, std::size_t y) const = 0;
};
} // namespace nr::dune2

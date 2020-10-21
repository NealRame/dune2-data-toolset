#pragma once

#include <optional>
#include <string>
#include <vector>

namespace nr::dune2 {
class ICN {
public:
    struct Info {
        unsigned int width;
        unsigned int height;
        unsigned int bitPerPixels;

        std::size_t tileSize() const
        { return (width*height*bitPerPixels)/8; }

        std::size_t paletteSize() const
        { return 1<<bitPerPixels; }
    };

    struct Tile {
        Info info;
        std::vector<uint8_t> paletteOffsets;
        std::vector<uint8_t> data;

        uint8_t operator[](std::size_t index) const;
    };

public:
    static std::optional<ICN> load(const std::string &filepath);

public:
    using const_iterator = std::vector<Tile>::const_iterator;

    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

private:
    Info info_;
    std::vector<Tile> tiles_;
};
} // namespace nr::dune2

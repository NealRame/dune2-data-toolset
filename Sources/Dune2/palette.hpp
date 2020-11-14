#pragma once

#include <optional>
#include <string>
#include <vector>

namespace nr::dune2 {
class Palette {
public:
    static std::optional<Palette> load(const std::string &filepath);

public:
    struct Color {
        uint8_t red{0};
        uint8_t green{0};
        uint8_t blue{0};
    };

public:
    Palette();

public:
    size_t size() const
    { return colors_.size(); }

    const Color &operator[](size_t index) const
    { return colors_[index]; }

    Color &operator[](size_t index)
    { return colors_[index]; }

public:
    using iterator = std::vector<Color>::iterator;
    using const_iterator = std::vector<Color>::const_iterator;

    iterator begin()
    { return colors_.begin(); }

    iterator end()
    { return colors_.end(); }

    const_iterator begin() const
    { return const_cast<Palette &>(*this).begin(); }

    const_iterator end() const
    { return const_cast<Palette &>(*this).end(); }

private:
    std::vector<Color> colors_;
};
}
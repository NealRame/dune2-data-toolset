#pragma once

#include <limits>
#include <string>
#include <vector>

namespace nr::dune2 {
class Palette {
public:
    struct Color {
        using Channel = uint8_t;
        static constexpr Channel ChannelMax = std::numeric_limits<Channel>::max();
        Channel red{0};
        Channel green{0};
        Channel blue{0};
        Channel alpha{0};
    };

public:
    Palette();

public:
    void load(const std::string &filepath);

public:
    size_t size() const
    { return colors_.size(); }

    const Color &at(size_t index) const
    { return colors_[index]; }

    Color &at(size_t index)
    { return colors_[index]; }

    const Color &operator[](size_t index) const
    { return at(index); }

    Color &operator[](size_t index)
    { return at(index); }

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
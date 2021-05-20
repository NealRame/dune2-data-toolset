#pragma once

#include <rapidjson/document.h>

#include <filesystem>
#include <limits>
#include <vector>

namespace nr::dune2 {
class BMP;
class Palette {
public:
    struct Color {
        using Channel = uint8_t;
        static constexpr Channel ChannelMax = std::numeric_limits<Channel>::max();
        Channel red{0};
        Channel green{0};
        Channel blue{0};
    };

public:
    Palette();

public:
    void loadFromPAL(const std::filesystem::path &);
    void loadFromJSON(const std::filesystem::path &);

public:
    rapidjson::Document toJSON() const;
    BMP toBMP() const;

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
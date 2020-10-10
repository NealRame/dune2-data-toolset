#pragma once

#include <Dune2/color.hpp>

#include <optional>
#include <string>
#include <vector>

namespace nr::dune2 {
class Palette {
public:
    static std::optional<Palette> load(const std::string &filepath);

public:
    std::size_t size() const
    { return colors_.size(); }

    const Color &operator[](std::size_t index) const
private:
    std::vector<Color> colors_;
};
}
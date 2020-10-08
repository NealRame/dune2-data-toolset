#pragma once

#include <Dune2/color.hpp>

#include <optional>
#include <string>
#include <vector>

namespace nr::dune2 {
class Palette {
public:
    static std::optional<Palette> load(const std::string &filepath);

private:
    std::vector<Color> colors_;
};
}
#pragma once

#include <optional>
#include <string>
#include <vector>

namespace nr::dune2 {
class Palette {
public:
    struct Color {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    };

public:
    static std::optional<Palette> load(const std::string &filepath);

private:
    std::vector<Color> colors_;
};
}
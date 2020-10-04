#pragma once

#include <filesystem>
#include <optional>
#include <vector>

namespace nr::dune2 {
class SHP {
public:
    enum Version {
        v100 = 100,
        v107 = 107,
    };

    struct Frame {
        std::size_t width;
        std::size_t height;
        std::vector<uint8_t> remapTable;
        std::vector<std::uint8_t> data;
    };

public:
    static std::optional<SHP> load(const std::string &filepath);

public:
    using const_iterator = std::vector<Frame>::const_iterator;

    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

private:
    std::vector<Frame> frames_;
};

} // namespace nr::dune2

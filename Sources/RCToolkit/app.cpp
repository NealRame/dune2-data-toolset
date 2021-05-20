#include "app.hpp"

#include <fmt/format.h>

#include <regex>

namespace nr {
namespace fs = std::filesystem;

bool
filepathMatch(
    const fs::path &filepath,
    const std::string extension
) {
    const auto ext = filepath.extension().string();
    return std::regex_match(ext, std::regex(
        fmt::format(R"(^\{}$)", extension),
        std::regex_constants::icase
    ));
}

template <>
void
load<dune2::Palette>(
    dune2::Palette &palette,
    const std::filesystem::path &source
) {
    if (filepathMatch(source, ".pal")) {
        palette.loadFromPAL(source);
    } else if (filepathMatch(source, ".json")) {
        palette.loadFromJSON(source);
    } else {
        throw CLI::Error(
            "Unsupported file",
            fmt::format("Unsupported file type: '{}'", source.extension().string()),
            CLI::ExitCodes::InvalidError
        );
    }
}

template <>
void
load<dune2::ImageSet>(
    dune2::ImageSet &tileset,
    const std::filesystem::path &source
) {
    if (filepathMatch(source, ".icn")) {
        tileset.loadFromICN(source);
    } else if (filepathMatch(source, ".shp")) {
        tileset.loadFromSHP(source);
    } else if (filepathMatch(source, ".json")) {
        tileset.loadFromJSON(source);
    } else if (filepathMatch(source, ".cps")) {
        dune2::Image image;
        image.loadFromCPS(source);
        tileset.push_back(std::move(image));
    } else {
        throw CLI::Error(
            "Unsupported file",
            fmt::format("Unsupported file type: '{}'", source.extension().string()),
            CLI::ExitCodes::InvalidError
        );
    }
}

template <>
void load<dune2::IconSet>(
    dune2::IconSet &iconset,
    const std::filesystem::path &source
) {
    if (filepathMatch(source, ".map")) {
        iconset.loadFromMAP(source);
    } else if (filepathMatch(source, ".json")) {
        iconset.loadFromJSON(source);
    } else {
        throw CLI::Error(
            "Unsupported file",
            fmt::format("Unsupported file type: '{}'", source.extension().string()),
            CLI::ExitCodes::InvalidError
        );
    }
}

} // namespace nr

#pragma once

#include <Dune2/palette.hpp>
#include <Dune2/tileset.hpp>

#include <filesystem>
#include <memory>

namespace nr::dune2 {

class Resource {
public:
    Resource();
    virtual ~Resource();

public:
    struct serialization_failure : public std::exception { };
    struct deserialization_failure : public std::exception { };

public:
    void addPalette(const Palette &);
    std::optional<Palette> getPalette(const std::string &) const;
    std::vector<std::string> getPaletteList() const;

    void addTileset(const Tileset &);
    std::optional<Tileset> getTileset(const std::string &) const;
    std::vector<std::string> getTilesetList() const;

public:
    void deserialize(const std::filesystem::path &);
    void serialize(const std::filesystem::path &) const;

private:
    struct impl;
    std::unique_ptr<impl> d;
};

} // namespace nr::dune2

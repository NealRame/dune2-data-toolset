#pragma once

#include <Dune2/palette.hpp>
#include <Dune2/tileset.hpp>
#include <Dune2/iconset.hpp>

#include <rapidjson/document.h>

#include <filesystem>
#include <memory>

namespace nr::dune2 {

class Resource {
public:
    struct PaletteNotFound: public std::exception {};

    struct ResourceNotFound: public std::exception {
        const std::string name;

        ResourceNotFound(const std::string &name)
            : name{name} {
        }

        virtual const char *what() const noexcept override
        { return name.c_str(); }
    };

public:
    Resource();
    virtual ~Resource();

public:
    struct serialization_failure : public std::exception { };
    struct deserialization_failure : public std::exception { };

public:
    bool hasPalette() const;
    Palette getPalette() const;
    void importPalette(const std::filesystem::path &);
    void removePalette();

    Tileset getTileset(const std::string &name) const;
    std::vector<std::string> getTilesetList() const;
    bool hasTileset(const std::string &name) const;
    void createTileset(const std::string &name);
    void removeTileset(const std::string &name);
    void importTileset(const std::string &name, const std::filesystem::path &);

    Iconset getIconset(const std::string &name) const;
    std::vector<std::string> getIconsetList() const;
    bool hasIconset(const std::string &name) const;
    void createIconset(const std::string &name);
    void removeIconset(const std::string &name);
    void importIconset(const std::string &name, const std::filesystem::path &);

    std::vector<std::string> getSoundsetList() const;
    std::vector<std::string> getSoundList(const std::string &soundset) const;
    std::vector<std::pair<std::string, std::string>> getAllSounds(const std::string &soundset) const;
    bool hasSoundset(const std::string &soundset) const;
    void createSoundset(const std::string &soundset);
    void removeSoundset(const std::string &soundset);
    void addSound(const std::string &soundset, const std::filesystem::path &filepath);

public:
    void deserialize(const std::filesystem::path &);
    void serialize(const std::filesystem::path &) const;

public:
    rapidjson::Document json_export() const;

private:
    struct impl;
    std::unique_ptr<impl> d;
};

} // namespace nr::dune2

#include <app.hpp>

#include <Dune2/bmp.hpp>
#include <Dune2/resource.hpp>

#include <fmt/format.h>

#include <regex>
#include <tuple>

namespace {

bool
filepath_match(const fs::path &filepath, const std::string extension) {
    const auto ext = filepath.extension().string();
    return std::regex_match(ext, std::regex(
        fmt::format(R"(^\{}$)", extension),
        std::regex_constants::icase
    ));
}

CLI::App_p
create_tileset_add_command(AppState &app_state) {
    auto cmd = std::make_shared<App>();

    cmd->name("add");
    cmd->description("Add a tileset to resources");
    cmd->add_option_function<fs::path>(
        "file",
        [&](const fs::path &dune2_tileset_path) {
            const auto name = dune2_tileset_path.stem().string();
            const auto ext  = dune2_tileset_path.extension().string();

            nr::dune2::Tileset tileset(name);

            if (filepath_match(dune2_tileset_path, ".icn")) {
                tileset.loadFromICN(dune2_tileset_path);
            }

            if (filepath_match(dune2_tileset_path, ".shp")) {
                tileset.loadFromSHP(dune2_tileset_path);
            }

            nr::dune2::Resource rc;
            rc.deserialize(app_state.dune2RCPath);
            rc.addTileset(tileset);
            rc.serialize(app_state.dune2RCPath);
        },
        "Path to .icn or .shp file"
    )->check(CLI::ExistingFile);

    return cmd;
}

CLI::App_p
create_tileset_list_command(AppState &app_state) {
    auto cmd = std::make_shared<App>();

    cmd->name("list");
    cmd->description("List tilesets");
    cmd->callback([&] {
        nr::dune2::Resource rc;
        rc.deserialize(app_state.dune2RCPath);
        const auto tileset_list = rc.getTilesetList();
        std:copy(
            tileset_list.begin(),
            tileset_list.end(),
            std::ostream_iterator<std::string>(std::cout, "\n")
        );
    });

    return cmd;
}

CLI::App_p
create_tileset_export_command(AppState &app_state) {
    using args_t = std::tuple<std::string, std::string>;

    auto cmd = std::make_shared<App>();
    auto args = std::make_shared<args_t>();

    cmd->name("export");
    cmd->description("Export tileset to bmp files");
    cmd->add_option_function<std::string>(
        "tileset_name",
        [args](const std::string &tileset_name) {
            std::get<0>(*args) = tileset_name;
        }
    )->required();
    cmd->add_option_function<std::string>(
        "palette_name",
        [args](const std::string &palette_name) {
            std::get<1>(*args) = palette_name;
        }
    )->required();
    cmd->callback([&app_state, args, cmd]{
        const auto &[tileset_name, palette_name] = *args;

        nr::dune2::Resource rc;
        rc.deserialize(app_state.dune2RCPath);

        const auto palette = rc.getPalette(palette_name);
        if (!palette) {
            throw CLI::Error("PaletteNotFoundError", "Palette not found!");
        }

        const auto tileset = rc.getTileset(tileset_name);
        if (!tileset) {
            throw CLI::Error("TilesetNotFoundError", "Tileset not found!");
        }

        int i = 0;
        for (auto &&tile: *tileset) {
            nr::dune2::BMP bmp(tile.getWidth(), tile.getHeight());
            bmp.drawSurface(0, 0, tile, *palette);
            bmp.store(fmt::format("{}#{}.bmp", tileset->getName(), ++i));
        }
    });

    return cmd;
}
} // namespace


CLI::App_p
createTilesetCommands(AppState &app_state) {
    auto cmd = std::make_shared<App>();

    cmd->name("tileset");
    cmd->description("Tileset commands");
    cmd->add_subcommand(create_tileset_add_command(app_state));
    cmd->add_subcommand(create_tileset_list_command(app_state));
    cmd->add_subcommand(create_tileset_export_command(app_state));

    return cmd;
}

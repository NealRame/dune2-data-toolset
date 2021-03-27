#include <app.hpp>

#include <Dune2/bmp.hpp>

#include <fmt/format.h>


namespace {

CLI::App_p
create_import_command(AppState &app_state) {
    struct CmdState {
        std::optional<std::string> name;
        std::vector<fs::path> sources;
    };

    auto cmd = std::make_shared<App>();
    auto cmd_state = std::make_shared<CmdState>();

    cmd->name("import");
    cmd->description("Import Dune2 sprites/tiles to resources");
    cmd->add_option_function<std::string>(
        "-n,--name",
        [cmd_state](const std::string &name) {
            cmd_state->name = name;
        },
        "Specify tileset name"
    );
    cmd->add_option_function<std::vector<fs::path>>(
        "SOURCES",
        [cmd_state](const std::vector<fs::path> &sources) {
            cmd_state->sources = sources;
        },
        "Path to Dune2 .icn or .shp files"
    )->required();
    cmd->callback([cmd, cmd_state, &app_state] {
        const auto name = cmd_state->name.value_or(
            cmd_state->sources.front().stem().string()
        );
        auto rc = app_state.resource();

        rc->createTileset(name);
        for (auto &&source: cmd_state->sources) {
            rc->importTileset(name, source);
        }
    });
    return cmd;
}

CLI::App_p
create_export_command(AppState &app_state) {
    struct CmdState {
        fs::path outputDirectory{fs::current_path()};
        std::string paletteNane;
        std::string tilesetName;
    };

    auto cmd = std::make_shared<App>();
    auto cmd_state = std::make_shared<CmdState>();

    cmd->name("export");
    cmd->description("Export tileset to bmp files");
    cmd->add_option_function<fs::path>(
        "-d,--output-directory",
        [cmd_state](const fs::path &output_directory) {
            cmd_state->outputDirectory = output_directory;
        },
        "Specify the output directory"
    );
    cmd->add_option_function<std::string>(
        "TILESET_NAME",
        [cmd_state](const std::string &tileset_name) {
            cmd_state->tilesetName = tileset_name;
        }
    )->required();
    cmd->add_option_function<std::string>(
        "PALETTE_NAME",
        [cmd_state](const std::string &palette_name) {
            cmd_state->paletteNane = palette_name;
        }
    )->required();
    cmd->callback([cmd, cmd_state, &app_state]{
        using fmt::format;
        const auto output_directory = cmd_state->outputDirectory;
        const auto rc = app_state.resource();
        const auto palette = rc->getPalette(cmd_state->paletteNane);
        const auto tileset = rc->getTileset(cmd_state->tilesetName);
        const auto tileset_name = tileset.getName();
        std::for_each(
            tileset.begin(),
            tileset.end(),
            [&, i = 0u](const auto &tile) mutable {
                const auto filename = format("{}#{}.bmp", tileset_name, ++i);
                nr::dune2::BMP bmp(tile.getWidth(), tile.getHeight());
                bmp.drawSurface(0, 0, tile, palette);
                bmp.store(output_directory/tileset_name);
            }
        );
    });

    return cmd;
}

CLI::App_p
create_remove_command(AppState &app_state) {
    auto cmd = std::make_shared<App>();

    cmd->name("remove");
    cmd->description("Remove tileset");
    cmd->add_option_function<std::string>(
        "TILESET_NAME",
        [&](const std::string &name) {
            auto rc = app_state.resource();
            if (!rc->hasTileset(name)) {
                throw CLI::Error(
                    "TilesetNotFound",
                    fmt::format("tileset '{}' not found!", name)
                );
            }
            rc->removeTileset(name);
        }
    );

    return cmd;
}

CLI::App_p
create_list_command(AppState &app_state) {
    auto cmd = std::make_shared<App>();

    cmd->name("list");
    cmd->description("List tilesets");
    cmd->callback([&] {
        const auto rc = app_state.resource();
        const auto tileset_list = rc->getTilesetList();
        std:copy(
            tileset_list.begin(),
            tileset_list.end(),
            std::ostream_iterator<std::string>(std::cout, "\n")
        );
    });

    return cmd;
}

} // namespace

CLI::App_p
createTilesetCommands(AppState &app_state) {
    auto cmd = std::make_shared<App>();

    cmd->name("tileset");
    cmd->description("Tileset commands");
    cmd->require_subcommand(1);
    cmd->add_subcommand(create_export_command(app_state));
    cmd->add_subcommand(create_import_command(app_state));
    cmd->add_subcommand(create_list_command(app_state));
    cmd->add_subcommand(create_remove_command(app_state));

    return cmd;
}

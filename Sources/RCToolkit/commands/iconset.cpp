#include <app.hpp>

#include <Dune2/bmp.hpp>

#include <fmt/format.h>

namespace {
CLI::App_p
create_import_command(AppState &app_state) {
    struct CmdState {
        bool force{false};
        std::optional<std::string> name;
        std::vector<fs::path> sources;
    };

    auto cmd = std::make_shared<App>();
    auto cmd_state = std::make_shared<CmdState>();

    cmd->name("import");
    cmd->description("Import Dune2 icon mapping to resources");
    cmd->add_option_function<std::string>(
        "-n,--name",
        [cmd_state](const std::string &name) {
            cmd_state->name = name;
        },
        "Specify tileset name to map"
    );
    cmd->add_option_function<std::vector<fs::path>>(
        "SOURCES",
        [cmd_state](const std::vector<fs::path> &sources) {
            cmd_state->sources = sources;
        },
        "Path to Dune2 .map files"
    )->required();
    cmd->callback([cmd, cmd_state, &app_state] {
        const auto name = cmd_state->name.value_or(
            cmd_state->sources.front().stem().string()
        );
        auto rc = app_state.resource();

        rc->createIconset(name);
        for (auto &&source: cmd_state->sources) {
            rc->importIconset(name, source);
        }
    });
    return cmd;
}

CLI::App_p
create_export_command(AppState &app_state) {
    struct CmdState {
        std::string paletteNane;
        std::string iconsetName;
    };

    auto cmd = std::make_shared<App>();
    auto cmd_state = std::make_shared<CmdState>();

    cmd->name("export");
    cmd->description("Export iconset to bmp files");
    cmd->add_option_function<std::string>(
        "ICONSET_NAME",
        [cmd_state](const std::string &iconset_name) {
            cmd_state->iconsetName = iconset_name;
        }
    )->required();
    cmd->callback([cmd, cmd_state, &app_state]{
        const auto rc = app_state.resource();
        const auto palette = rc->getPalette();
        const auto tileset = rc->getTileset(cmd_state->iconsetName);
        const auto iconset = rc->getIconset(cmd_state->iconsetName);

        int i = 0;
        for (auto &&icon: iconset) {
            const auto surface = icon.getSurface(tileset);
            nr::dune2::BMP bmp(surface.getWidth(), surface.getHeight());
            bmp.drawSurface(0, 0, surface, palette);
            bmp.store(fmt::format("{}#{}.bmp", tileset.getName(), ++i));
        }
    });

    return cmd;
}

CLI::App_p
create_remove_command(AppState &app_state) {
    auto cmd = std::make_shared<App>();

    cmd->name("remove");
    cmd->description("Remove iconset");
    cmd->add_option_function<std::string>(
        "ICONSET_NAME",
        [&](const std::string &name) {
            auto rc = app_state.resource();
            if (!rc->hasIconset(name)) {
                throw CLI::Error(
                    "IconsetNotFound",
                    fmt::format("iconset '{}' not found!", name)
                );
            }
            rc->removeIconset(name);
        }
    );

    return cmd;
}

CLI::App_p
create_list_command(AppState &app_state) {
    auto cmd = std::make_shared<App>();

    cmd->name("list");
    cmd->description("List iconsets");
    cmd->callback([&] {
        const auto rc = app_state.resource();
        const auto iconset_list = rc->getIconsetList();
        std:copy(
            iconset_list.begin(),
            iconset_list.end(),
            std::ostream_iterator<std::string>(std::cout, "\n")
        );
    });

    return cmd;
}
} // namespace

CLI::App_p
createIconsetCommands(AppState &app_state) {
    auto cmd = std::make_shared<App>();

    cmd->name("iconset");
    cmd->description("Iconset commands");
    cmd->require_subcommand(1);
    cmd->add_subcommand(create_export_command(app_state));
    cmd->add_subcommand(create_import_command(app_state));
    cmd->add_subcommand(create_list_command(app_state));
    cmd->add_subcommand(create_remove_command(app_state));

    return cmd;
}

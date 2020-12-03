#include <app.hpp>

#include <Dune2/bmp.hpp>

#include <fmt/format.h>

namespace {

CLI::App_p
create_import_command(AppState &app_state) {
    struct CmdState {
        bool force{false};
        std::string name;
        fs::path dune2InputFilepath;
    };

    auto cmd = std::make_shared<App>();
    auto cmd_state = std::make_shared<CmdState>();

    cmd->name("import");
    cmd->description("Import a Dune2 palette to resources");
    cmd->add_flag_function(
        "-f,--force",
        [cmd_state](auto count) {
            cmd_state->force = (count != 0);
        },
        "Force overwrite if a palette with same name already exist in resources"
    );
    cmd->add_option_function<std::string>(
        "-n,--name",
        [cmd_state](const std::string &name) {
            cmd_state->name = name;
        },
        "Specify palette name"
    );
    cmd->add_option_function<fs::path>(
        "PAL_FILE_PATH",
        [cmd_state](const fs::path &filepath) {
            cmd_state->dune2InputFilepath = filepath;
        },
        "Path to Dune2 .pal file"
    )->check(CLI::ExistingFile)->required();
    cmd->callback([cmd, cmd_state, &app_state] {
        auto rc = app_state.resource();
        const auto name = cmd_state->name.empty()
            ? cmd_state->dune2InputFilepath.stem().string()
            : cmd_state->name;

        if (rc->hasPalette(name)) {
            if (!cmd_state->force) {
                throw CLI::Error(
                    "PaletteOverwrite",
                    fmt::format("palette '{}' already exist!", name)
                );
            }
            rc->removePalette(name);
        }

        rc->importPalette(name, cmd_state->dune2InputFilepath);
    });
    return cmd;
}


CLI::App_p
create_export_command(AppState &app_state) {
    auto cmd = std::make_shared<App>();

    cmd->name("export");
    cmd->description("Export a palette to a bmp file");
    cmd->add_option_function<std::string>(
        "PALETTE_NAME",
        [&](const std::string &name) {
            const auto rc = app_state.resource();
            const auto palette = rc->getPalette(name);

            nr::dune2::BMP bmp(256, 256);
            for (auto i = 0; i < 256; ++i) {
                const auto row = i/16;
                const auto col = i%16;
                bmp.fillRect(col*16, row*16, 16, 16, palette.at(i));
            }

            bmp.store(fmt::format("{}.bmp", name));
        }
    );

    return cmd;
}

CLI::App_p
create_remove_command(AppState &app_state) {
    auto cmd = std::make_shared<App>();

    cmd->name("remove");
    cmd->description("Remove palette");
    cmd->add_option_function<std::string>(
        "PALETTE_NAME",
        [&](const std::string &name) {
            auto rc = app_state.resource();
            rc->removePalette(name);
        }
    );

    return cmd;
}

CLI::App_p
create_list_command(AppState &app_state) {
    auto cmd = std::make_shared<App>();

    cmd->name("list");
    cmd->description("List palettes");
    cmd->callback([&] {
        const auto rc = app_state.resource();
        const auto palettes = rc->getPaletteList();
        std:copy(
            palettes.begin(),
            palettes.end(),
            std::ostream_iterator<std::string>(std::cout, "\n")
        );
    });

    return cmd;
}

} // namespace

CLI::App_p
createPaletteCommands(AppState &app_state) {
    auto cmd = std::make_shared<App>();

    cmd->name("palette");
    cmd->description("Palette commands");
    cmd->require_subcommand(1);
    cmd->add_subcommand(create_export_command(app_state));
    cmd->add_subcommand(create_import_command(app_state));
    cmd->add_subcommand(create_list_command(app_state));
    cmd->add_subcommand(create_remove_command(app_state));

    return cmd;
}
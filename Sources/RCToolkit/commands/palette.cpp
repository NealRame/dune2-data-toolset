#include <app.hpp>

#include <Dune2/bmp.hpp>
#include <Dune2/palette.hpp>

#include <fmt/format.h>

#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>

#include <filesystem>

namespace {
namespace fs = std::filesystem;

using nr::AppState;

using rapidjson::OStreamWrapper;

using PrettyWriter = rapidjson::PrettyWriter<OStreamWrapper>;
using Writer = rapidjson::Writer<OStreamWrapper>;

CLI::App_p
create_create_command(AppState &app_state) {
    struct CmdState {
        bool pretty{false};
        fs::path inputFilepath;
        std::optional<fs::path> outputFilepath;
    };

    auto cmd = std::make_shared<App>();
    auto cmd_state = std::make_shared<CmdState>();

    cmd->name("create");
    cmd->description("Convert a Dune2 palette into a JSON file");

    cmd->add_flag_function(
        "-p,--pretty",
        [cmd_state](auto count) {
            cmd_state->pretty = (count != 0);
        },
        "Enable pretty output"
    );

    cmd->add_option_function<fs::path>(
        "-o,--output-file",
        [cmd_state](const fs::path &outputFilepath) {
            cmd_state->outputFilepath = outputFilepath;
        },
        "Specify the output file"
    );

    cmd->add_option_function<fs::path>(
        "PAL_FILE_PATH",
        [cmd_state](const fs::path &filepath) {
            cmd_state->inputFilepath = filepath;
        },
        "Path to Dune2 .pal file"
    )->check(CLI::ExistingFile)->required();

    cmd->callback([cmd, cmd_state, &app_state] {
        nr::dune2::Palette pal;

        pal.loadFromPAL(cmd_state->inputFilepath);
        const auto json = pal.toJSON();

        if (cmd_state->outputFilepath) {
            std::ofstream ofs(cmd_state->outputFilepath.value());
            nr::flushJSON(json, cmd_state->pretty, ofs);
        } else {
            nr::flushJSON(json, cmd_state->pretty, std::cout);
        }
    });
    return cmd;
}

CLI::App_p
create_extract_command(AppState &app_state) {
    struct CmdState {
        bool pretty{false};
        fs::path inputFilepath;
        fs::path outputFilepath{"palette.bmp"};
    };

    auto cmd = std::make_shared<App>();
    auto cmd_state = std::make_shared<CmdState>();

    cmd->name("extract");
    cmd->description("Convert a Dune2 palette into a BMP file");

    cmd->add_option_function<fs::path>(
        "-o,--output-file",
        [cmd_state](const fs::path &outputFilepath) {
            cmd_state->outputFilepath = outputFilepath;
        },
        "Specify the output file"
    );

    cmd->add_option_function<fs::path>(
        "PAL_FILE_PATH",
        [cmd_state](const fs::path &filepath) {
            cmd_state->inputFilepath = filepath;
        },
        "Path to Dune2 .pal or .json palette file"
    )->check(CLI::ExistingFile)->required();

    cmd->callback([cmd, cmd_state, &app_state] {
        nr::dune2::Palette pal;
        nr::load(pal, cmd_state->inputFilepath);
        pal.toBMP().store(cmd_state->outputFilepath);
    });

    return cmd;
}

} // namespace

CLI::App_p
createPaletteCommands(nr::AppState &app_state) {
    auto cmd = std::make_shared<App>();

    cmd->name("palette");
    cmd->description("Palette commands");
    cmd->require_subcommand(1);
    cmd->add_subcommand(create_create_command(app_state));
    cmd->add_subcommand(create_extract_command(app_state));

    return cmd;
}

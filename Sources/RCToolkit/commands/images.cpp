#include <app.hpp>

#include <Dune2/bmp.hpp>

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
        bool force{false};
        bool pretty{false};
        std::vector<fs::path> sources;
        fs::path outputFilepath{"images.json"};
    };

    auto cmd = std::make_shared<App>();
    auto cmd_state = std::make_shared<CmdState>();

    cmd->name("create");
    cmd->description("Create a Dune2 image lib from the given files");

    cmd->add_flag_function(
        "-f,--force",
        [cmd_state](auto count) {
            cmd_state->force = (count != 0);
        },
        "Force overwrite if palette already exists"
    );
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
    cmd->add_option_function<std::vector<fs::path>>(
        "SOURCES",
        [cmd_state](const std::vector<fs::path> &sources) {
            cmd_state->sources = sources;
        },
        "Path to Dune2 .icn or .shp files"
    )->required();

    cmd->callback([cmd, cmd_state, &app_state] {
        nr::dune2::ImageSet tileset;
        for (auto &&source: cmd_state->sources) {
            nr::load(tileset, source);
        }

        std::ofstream ofs(cmd_state->outputFilepath);
        OStreamWrapper osw(ofs);
        const auto json = tileset.toJSON();

        if (cmd_state->pretty) {
            PrettyWriter writer(osw);
            json.Accept(writer);
        } else {
            Writer writer(osw);
            json.Accept(writer);
        }
    });
    return cmd;
}

CLI::App_p
create_extract_command(AppState &app_state) {
    struct CmdState {
        fs::path paletteFilepath;
        std::vector<fs::path> sources;
        fs::path outputDirectory{fs::current_path()};
    };

    auto cmd = std::make_shared<App>();
    auto cmd_state = std::make_shared<CmdState>();

    cmd->name("extract");
    cmd->description("Extract image from images lib");

    cmd->add_option_function<fs::path>(
        "-d,--output-directory",
        [cmd_state](const fs::path &output_directory) {
            cmd_state->outputDirectory = output_directory;
        },
        "Specify the output directory"
    )->check(CLI::ExistingDirectory);

    cmd->add_option_function<fs::path>(
        "PALETTE",
        [cmd_state](const fs::path &paletteFilepath) {
            cmd_state->paletteFilepath = paletteFilepath;
        },
        "Path to Dune2 .pal or .json file"
    )->required()->check(CLI::ExistingFile);

    cmd->add_option_function<std::vector<fs::path>>(
        "SOURCES",
        [cmd_state](const std::vector<fs::path> &sources) {
            cmd_state->sources = sources;
        },
        "Path to Dune2 .cps, .icn, .shp or .json files"
    )->required()->check(CLI::ExistingFile);

    cmd->callback([cmd, cmd_state, &app_state]{
        using fmt::format;
        const auto output_directory = cmd_state->outputDirectory;

        nr::dune2::Palette palette;
        nr::load(palette, cmd_state->paletteFilepath);

        nr::dune2::ImageSet images;
        for (auto &&source: cmd_state->sources) {
            nr::load(images, source);
        }

        std::for_each(
            images.begin(),
            images.end(),
            [&, i = 0u](const auto &tile) mutable {
                const auto filename = format("{}.bmp", ++i);
                nr::dune2::BMP bmp(tile.getWidth(), tile.getHeight());
                bmp.drawSurface(0, 0, tile, palette);
                bmp.store(output_directory/filename);
            }
        );
    });

    return cmd;
}
} // namespace

CLI::App_p
createTilesetCommands(nr::AppState &app_state) {
    auto cmd = std::make_shared<App>();

    cmd->name("images");
    cmd->description("Image set commands");
    cmd->require_subcommand(1);
    cmd->add_subcommand(create_create_command(app_state));
    cmd->add_subcommand(create_extract_command(app_state));

    return cmd;
}

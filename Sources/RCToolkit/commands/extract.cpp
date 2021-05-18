#include <app.hpp>

#include <Dune2/bmp.hpp>
#include <Dune2/io.hpp>
#include <Dune2/palette.hpp>

#include <fmt/format.h>
#include <rapidjson/document.h>

namespace {

rapidjson::Document
loadJSON(std::istream &in) {
    const auto src = nr::dune2::io::readAll(in);
    rapidjson::Document doc;
    doc.Parse(src.c_str());
    return doc;
}

nr::dune2::Palette
load_palette(const fs::path &inputFilePath) {
    std::ifstream input(inputFilePath);
    nr::dune2::Palette palette;

    palette.loadFromJSON(loadJSON(input));
    return palette;
}

std::vector<nr::dune2::Tileset>
load_tilesets(const fs::path &inputFilePath) {
    std::ifstream input(inputFilePath);
    const auto doc = loadJSON(input);

    std::vector<nr::dune2::Tileset> tilesets;
    for (auto &&m : doc.GetObject()) {
        nr::dune2::Tileset tileset;
        tileset.setName(std::string(m.name.GetString()));
        tileset.loadFromJSON(m.value);
        tilesets.emplace_back(std::move(tileset));
    }

    return tilesets;
}

template<typename ColorContainer>
nr::dune2::BMP
create_color_palette_swatches(
    const ColorContainer &colors,
    unsigned int columns,
    unsigned int rows
) {
    nr::dune2::BMP bmp(columns*16, rows*16);
    for (auto i = 0; i < colors.size(); ++i) {
        const auto row = i/16;
        const auto col = i%16;
        bmp.fillRect(col*16, row*16, 16, 16, colors.at(i));
    }
    return bmp;
}

CLI::App_p
create_extract_palette_command(AppState &app_state) {
    struct CmdState {
        bool force{false};
        fs::path outputDirectory{fs::current_path()};
        fs::path paletteInputFilePath;
    };

    auto cmd = std::make_shared<App>();
    auto cmd_state = std::make_shared<CmdState>();

    cmd->name("palette");
    cmd->description("Extract palette to a BMP file");
    cmd->add_option_function<fs::path>(
        "-d,--output-directory",
        [cmd_state](const fs::path &outputDirectory) {
            cmd_state->outputDirectory = outputDirectory;
        },
        "Specify the output directory"
    );
    cmd->add_option_function<fs::path>(
        "PALETTE_JSON",
        [cmd_state](fs::path paletteInputFilePath) {
            cmd_state->paletteInputFilePath = paletteInputFilePath;
        },
        "Path to Dune2 palette.json file"
    )->required();
    cmd->callback([cmd, cmd_state, &app_state] {
        using nr::dune2::Palette;
        using nr::dune2::BMP;

        Palette palette = load_palette(cmd_state->paletteInputFilePath);
        BMP bmp = create_color_palette_swatches(palette, 16, 16);

        const auto output_file_name = fmt::format("{}.bmp", cmd_state->paletteInputFilePath.stem().string());
        const auto output_file_path = cmd_state->outputDirectory/output_file_name;

        fs::create_directories(cmd_state->outputDirectory);
        bmp.store(output_file_path);
    });

    return cmd;
}

CLI::App_p
create_extract_tilesets_command(AppState &app_state) {
    struct CmdState {
        fs::path outputDirectory{fs::current_path()};
        fs::path paletteInputFilePath;
        fs::path tilesetsInputFilePath;
    };

    auto cmd = std::make_shared<App>();
    auto cmd_state = std::make_shared<CmdState>();

    cmd->name("tilesets");
    cmd->description("Export tilesets to BMP files");
    cmd->add_option_function<fs::path>(
        "-d,--output-directory",
        [cmd_state](const fs::path &outputDirectory) {
            cmd_state->outputDirectory = outputDirectory;
        },
        "Specify the output directory"
    );
    cmd->add_option_function<fs::path>(
        "PALETTE_JSON",
        [cmd_state](fs::path paletteInputFilePath) {
            cmd_state->paletteInputFilePath = paletteInputFilePath;
        },
        "Path to Dune2 palette.json file"
    )->required();
    cmd->add_option_function<fs::path>(
        "TILESETS_JSON",
        [cmd_state](fs::path tilesetsInputFilePath) {
            cmd_state->tilesetsInputFilePath = tilesetsInputFilePath;
        },
        "Path to Dune2 tilesets.json file"
    )->required();
    cmd->callback([cmd, cmd_state, &app_state] {
        using fmt::format;
        using nr::dune2::BMP;
        using nr::dune2::Palette;
        using nr::dune2::Tileset;

        const auto palette = load_palette(cmd_state->paletteInputFilePath);
        const auto tilesets = load_tilesets(cmd_state->tilesetsInputFilePath);

        for (const auto &tileset: tilesets) {
            std::for_each(
                tileset.begin(),
                tileset.end(),
                [&, tile_index = 0u](const auto &tile) mutable {
                    if (tileset.getName() == "Terrain" && tile_index == 19) {
                        const auto &data = tile.getData();
                        for (auto i = 0; i < data.size(); ++i) {
                            if (i > 0 && i % 16 == 0) {
                                std::cout << format("\n0x{:0>2x}, ", (unsigned char)data.at(i));
                            } else {
                                std::cout << format("0x{:0>2x}, ", (unsigned char)data.at(i));
                            }
                        }
                        std::cout << std::endl;
                        std::cout << std::endl;
                    }
                    tile_index += 1;


                    const auto file_name = format("{}.bmp", tile_index);
                    const auto file_path = cmd_state->outputDirectory/tileset.getName();

                    // Create output directory
                    fs::create_directories(file_path);

                    // export tile as BMP
                    BMP bmp(tile.getWidth(), tile.getHeight());
                    bmp.drawSurface(0, 0, tile, palette);
                    bmp.store(file_path/file_name);

                    // export remap table as BMP if present
                    if (tile.hasRemapTable()) {
                        const auto remap_file_name = format("{}-remap.bmp", tile_index);
                        std::string remap = tile.getRemapTableData();
                        std::vector<Palette::Color> colors;
                        std::transform(
                            remap.begin(),
                            remap.end(),
                            std::back_inserter(colors),
                            [&palette](std::size_t remap_index) {
                                return palette.at(remap_index);
                            }
                        );

                        BMP remap_bmp = create_color_palette_swatches(colors, remap.size(), 1);
                        remap_bmp.store(file_path/remap_file_name);
                    }

                }
            );
        }
    });

    return cmd;
}
} // namespace

CLI::App_p
createExtractCommands(AppState &app_state) {
    auto cmd = std::make_shared<App>();

    cmd->name("extract");
    cmd->description("Extract data from JSON datasets");
    cmd->require_subcommand(1);
    cmd->add_subcommand(create_extract_palette_command(app_state));
    cmd->add_subcommand(create_extract_tilesets_command(app_state));

    return cmd;
}

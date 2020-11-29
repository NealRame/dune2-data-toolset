#include <app.hpp>

#include <Dune2/bmp.hpp>
#include <Dune2/resource.hpp>
#include <Dune2/palette.hpp>

#include <fmt/core.h>

namespace {
CLI::App_p
create_palette_add_command(AppState &app_state) {
    auto cmd = std::make_shared<App>();

    cmd->name("add");
    cmd->description("Add a palette to resources");
    cmd->add_option_function<fs::path>(
        "file",
        [&](const fs::path &dune2_pal_path) {
            const auto name = dune2_pal_path.stem();
            nr::dune2::Palette palette(name.string());
            palette.load(dune2_pal_path);

            nr::dune2::Resource rc;
            if (fs::is_regular_file(app_state.dune2RCPath)) {
                rc.deserialize(app_state.dune2RCPath);
            }
            rc.addPalette(palette);
            rc.serialize(app_state.dune2RCPath);
        },
        "Path to .pal file"
    )->check(CLI::ExistingFile);

    return cmd;
}

CLI::App_p
create_palette_list_command(AppState &app_state) {
    auto cmd = std::make_shared<App>();

    cmd->name("list");
    cmd->description("List palettes");
    cmd->callback([&] {
        nr::dune2::Resource rc;
        rc.deserialize(app_state.dune2RCPath);
        const auto palette_list = rc.getPaletteList();
        std:copy(
            palette_list.begin(),
            palette_list.end(),
            std::ostream_iterator<std::string>(std::cout, "\n")
        );
    });

    return cmd;
}

CLI::App_p
create_palette_export_command(AppState &app_state) {
    auto cmd = std::make_shared<App>();

    cmd->name("export");
    cmd->description("Export a palette to a bmp file");
    cmd->add_option_function<std::string>(
        "palette_name",
        [&](const std::string &name) {
            nr::dune2::Resource rc;
            rc.deserialize(app_state.dune2RCPath);

            if (auto palette = rc.getPalette(name)) {
                nr::dune2::BMP bmp(256, 256);
                for (auto i = 0; i < 256; ++i) {
                    const auto row = i/16;
                    const auto col = i%16;
                    bmp.fillRect(col*16, row*16, 16, 16, palette->at(i));
                }

                bmp.store(fmt::format("{}.bmp", name));
            }
        }
    );

    return cmd;
}

} // namespace



CLI::App_p
createPaletteCommands(AppState &app_state) {
    auto cmd = std::make_shared<App>();

    cmd->name("palette");
    cmd->description("Palette commands");
    cmd->add_subcommand(create_palette_add_command(app_state));
    cmd->add_subcommand(create_palette_list_command(app_state));
    cmd->add_subcommand(create_palette_export_command(app_state));

    return cmd;
}
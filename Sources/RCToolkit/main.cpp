#include <app.hpp>

#include <fmt/format.h>

CLI::App_p createExportCommand(AppState &);
CLI::App_p createPaletteCommands(AppState &);
CLI::App_p createTilesetCommands(AppState &);
CLI::App_p createIconsetCommands(AppState &);
CLI::App_p createSoundsetCommands(AppState &);

int
main(int argc, char const *argv[]) {
    App app{"Manage Dune2 resources file"};
    AppState app_state;

    app.add_option(
        "-i,--rc-file",
        app_state.dune2RCPath,
        "A Dune2 resource file"
    );
    app.add_flag(
        "-v,--verbose",
        app_state.verbose,
        "Produce verbose output"
    );

    app.require_subcommand(1);
    app.add_subcommand(createExportCommand(app_state));
    app.add_subcommand(createPaletteCommands(app_state));
    app.add_subcommand(createTilesetCommands(app_state));
    app.add_subcommand(createIconsetCommands(app_state));
    app.add_subcommand(createSoundsetCommands(app_state));

    try {
        app.parse((argc), (argv));
    } catch (const CLI::Error &e) {
        return app.exit(e);
    } catch (const nr::dune2::Resource::ResourceNotFound &e) {
        return app.exit(CLI::Error(
            "ResourceNotFound",
            fmt::format("Resource '{}' not found", e.name)
        ));
    }

    return 0;
}

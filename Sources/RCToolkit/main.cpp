#include <app.hpp>

#include <fmt/format.h>

CLI::App_p createPaletteCommands(nr::AppState &);
CLI::App_p createTilesetCommands(nr::AppState &);
CLI::App_p createIconsetCommands(nr::AppState &);

int
main(int argc, char const *argv[]) {
    App app{"Manage Dune2 resources file"};
    nr::AppState app_state;

    app.add_flag(
        "-v,--verbose",
        app_state.verbose,
        "Produce verbose output"
    );

    app.require_subcommand(1);

    app.add_subcommand(createPaletteCommands(app_state));
    app.add_subcommand(createTilesetCommands(app_state));
    app.add_subcommand(createIconsetCommands(app_state));

    try {
        app.parse((argc), (argv));
    } catch (const CLI::Error &e) {
        return app.exit(e);
    }

    return 0;
}

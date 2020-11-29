#include <app.hpp>

CLI::App_p createTilesetCommands(AppState &);
CLI::App_p createPaletteCommands(AppState &);

int
main(int argc, char const *argv[]) {
    App app{"Manage Dune2 resources file"};
    AppState app_state;

    app.add_option(
        "-i,--dune2-rc-file",
        app_state.dune2RCPath,
        "A Dune2 resource file"
    );
    app.add_flag(
        "-v,--verbose",
        app_state.verbose,
        "Produce verbose output"
    );

    app.require_subcommand(1);
    app.add_subcommand(createPaletteCommands(app_state));
    app.add_subcommand(createTilesetCommands(app_state));

    CLI11_PARSE(app, argc, argv);

    return 0;
}

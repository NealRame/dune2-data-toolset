#include <app.hpp>

#include <Dune2/bmp.hpp>

#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>

#include <fmt/format.h>

using rapidjson::OStreamWrapper;

using PrettyWriter = rapidjson::PrettyWriter<OStreamWrapper>;
using Writer = rapidjson::Writer<OStreamWrapper>;

CLI::App_p
createExportCommand(AppState &app_state) {
    struct CmdState {
        std::optional<fs::path> outputDirectory;
        bool pretty{false};
    };

    auto cmd = std::make_shared<App>();
    auto cmd_state = std::make_shared<CmdState>();

    cmd->name("export");
    cmd->description("Export resources to json files");
    cmd->add_option_function<fs::path>(
        "-d,--output-directory",
        [cmd_state](const fs::path &output_directory) {
            cmd_state->outputDirectory = output_directory;
        },
        "Specify the output directory"
    );
    cmd->add_flag_function(
        "-p,--pretty",
        [cmd_state](auto count) {
            cmd_state->pretty = true;
        },
        "Pretty print"
    );
    cmd->callback([cmd, cmd_state, &app_state] {
        const auto output_directory = cmd_state->outputDirectory.value_or(fs::current_path());
        const auto rc = app_state.resource();
        const auto doc = rc->json_export();

        for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
            const auto name = fmt::format("{}.json", it->name.GetString());
            std::ofstream ofs(output_directory/name);
            OStreamWrapper osw(ofs);

            if (cmd_state->pretty) {
                PrettyWriter writer(osw);
                it->value.Accept(writer);
            } else {
                Writer writer(osw);
                it->value.Accept(writer);
            }
        }
    });
    
    return cmd;
}

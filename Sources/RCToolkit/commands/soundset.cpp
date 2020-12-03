#include <app.hpp>

#include <Dune2/resource.hpp>

#include <fmt/format.h>

namespace {

void
add_sounds_to_soundset(
    nr::dune2::Resource &rc,
    const std::string &soundset,
    const fs::path &source) {
    if (fs::is_directory(source)) {
        for (auto &&entry: fs::directory_iterator(source)) {
            add_sounds_to_soundset(rc, soundset, entry);
        }
    } else {
        rc.addSound(soundset, source);
    }
}

CLI::App_p
create_add_command(AppState &app_state) {
    struct CmdState {
        bool force{false};
        std::optional<std::string> name;
        std::vector<fs::path> sources;
    };

    auto cmd = std::make_shared<App>();
    auto cmd_state = std::make_shared<CmdState>();

    cmd->name("add");
    cmd->description("Add a soundset to resources");
    cmd->add_flag_function(
        "-f,--force",
        [cmd_state](auto count) {
            cmd_state->force = (count != 0);
        },
        "Force overwrite if a soundset with same name already exist in resources"
    );
    cmd->add_option_function<std::string>(
        "-n,--name",
        [cmd_state](const std::string &name) {
            cmd_state->name = name;
        },
        "Specify soundset name"
    );
    cmd->add_option_function<std::vector<fs::path>>(
        "SOURCES",
        [cmd_state](const std::vector<fs::path> &sources) {
            cmd_state->sources = sources;
        }
    )->required();
    cmd->callback([cmd, cmd_state, &app_state] {
        auto rc = app_state.resource();
        const auto name = cmd_state->name.value_or(
            cmd_state->sources.front().stem().string()
        );

        if (rc->hasSoundset(name)) {
            if (!cmd_state->force) {
                throw CLI::Error(
                    "SoundsetOverwrite",
                    fmt::format("soundset '{}' already exist!", name)
                );
            }
            rc->removeSoundset(name);
        }

        rc->createSoundset(name);
        for (auto &&source: cmd_state->sources) {
            add_sounds_to_soundset(*rc, name, source);
        }
    });

    return cmd;
}

CLI::App_p
create_remove_command(AppState &app_state) {
    auto cmd = std::make_shared<App>();

    cmd->name("remove");
    cmd->description("Remove soundset");
    cmd->add_option_function<std::string>(
        "SOUNDSET_NAME",
        [&](const std::string &name) {
            auto rc = app_state.resource();
            if (!rc->hasSoundset(name)) {
                throw CLI::Error(
                    "SoundsetNotFound",
                    fmt::format("soundset '{}' not found!", name)
                );
            }
            rc->removeSoundset(name);
        }
    );

    return cmd;
}

CLI::App_p
create_list_command(AppState &app_state) {
    struct CmdState {
        std::optional<std::string> soundsetName;
    };

    auto cmd = std::make_shared<CLI::App>();
    auto cmd_state = std::make_shared<CmdState>();

    cmd->name("list");
    cmd->description("List soundsets");
    cmd->add_option_function<std::string>(
        "SOUNDSET_NAME",
        [cmd_state](const std::string &soundset_name) {
            cmd_state->soundsetName = soundset_name;
        }
    );
    cmd->callback([cmd_state, &app_state] {
        const auto rc = app_state.resource();
        const auto soundset_list = cmd_state->soundsetName.has_value()
            ? rc->getSoundList(cmd_state->soundsetName.value())
            : rc->getSoundsetList();
        std:copy(
            soundset_list.begin(),
            soundset_list.end(),
            std::ostream_iterator<std::string>(std::cout, "\n")
        );
    });

    return cmd;
}

} // namespace

CLI::App_p
createSoundsetCommands(AppState &app_state) {
    auto cmd = std::make_shared<App>();

    cmd->name("soundset");
    cmd->description("Soundset commands");
    cmd->require_subcommand(1);
    cmd->add_subcommand(create_add_command(app_state));
    cmd->add_subcommand(create_list_command(app_state));
    cmd->add_subcommand(create_remove_command(app_state));

    return cmd;
}
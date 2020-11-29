#pragma once

#include <CLI/CLI.hpp>
#include <filesystem>

namespace fs = std::filesystem;

using CLI::App;

struct AppState {
    unsigned int verbose{0};
    fs::path dune2RCPath{fs::current_path()/"dune2.rc"};
};
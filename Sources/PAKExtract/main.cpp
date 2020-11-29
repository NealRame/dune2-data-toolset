#include <Dune2/io.hpp>
#include <Dune2/pak.hpp>

#include <CLI/CLI.hpp>

#include <fmt/format.h>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

int
main(int argc, char const *argv[]) {
    CLI::App app{"Dune2 PAK file extractor"};

    bool list{false};
    bool verbose{false};
    fs::path output_dir{fs::current_path()};
    nr::dune2::PAK pak;

    app.add_flag("-l,--list", list, "List file");
    app.add_flag("-v,--verbose", verbose, "Produce verbose output");
    app.add_option("-d,--output-dir", output_dir, "Set output directory");

    app.add_option_function<fs::path>(
        "filepath",
        [&](const fs::path &filepath) {
            try {
                pak.load(filepath);
            } catch (...) {
                throw CLI::Error(
                    "PAKFailure",
                    fmt::format("Failed to load '{}'", filepath.string())
                );
            }
        },
        "Path to an existing PAK file"
    )->required()->check(CLI::ExistingFile);

    CLI11_PARSE(app, argc, argv);

    if (list) {
        for (const auto &entry: pak) {
            std::cout << entry.name << " " << entry.size << std::endl;
        }
    } else {
        fs::create_directories(output_dir);
        for (const auto &entry: pak) {
            auto out = std::fstream(
                output_dir/entry.name,
                std::ios::binary | std::ios::out | std::ios::trunc
            );

            if (verbose) {
                std::cerr << "extracting " << entry.name << " ... " << std::endl;
            }

            out.write(entry.read().data(), entry.size);
        }
    }

    return 0;
}

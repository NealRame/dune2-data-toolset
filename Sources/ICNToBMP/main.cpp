#include <Dune2/bmp.hpp>
#include <Dune2/icn.hpp>

#include <Dune2/io.hpp>
#include <Dune2/palette.hpp>

#include <boost/format.hpp>
#include <boost/program_options.hpp>

#include <filesystem>
#include <iostream>

namespace po = boost::program_options;
namespace fs = std::filesystem;

int
main(int argc, char **argv) {
    po::options_description options("OPTIONS");
    options.add_options()
        ("help,h", po::bool_switch(), "Display help message and exit.")
        ("verbose,v", po::bool_switch(), "Enable verbosity.")
        (
            "output-dir,d",
            po::value<fs::path>()->default_value(fs::current_path()),
            "Set output directory."
        );

    po::options_description hidden;
    hidden.add_options()
        ("input-pal-file", po::value<fs::path>(), "PAL file path.")
        ("input-map-file", po::value<fs::path>(), "MAP file path.")
        ("input-icn-file", po::value<fs::path>(), "ICN file path.");

    po::positional_options_description args;
    args
        .add("input-pal-file", 1)
        .add("input-map-file", 1)
        .add("input-icn-file", 1);

    po::options_description cli;
    cli.add(options).add(hidden);

    po::variables_map vm;
    po::store(
        po::command_line_parser(argc, argv)
            .options(cli)
            .positional(args)
            .run(),
        vm
    );
    po::notify(vm);

    const auto usage = [&](std::ostream &out) -> std::ostream & {
        return out
            << "Usage: " << fs::path(argv[0]).stem().string()
            << " [OPTIONS] PAL_FILE_PATH ICN_FILE_PATH"
            << "\n\n"
            << options
            << std::endl;
    };

    const auto verbose = [&]() {
        return vm["verbose"].as<bool>();
    };

    auto output_file_name = [&vm, tile_index = 1]() mutable {
        using boost::format;
        const auto shp_file_name = vm["input-icn-file"].as<fs::path>();
        return (format("%1%#%2%.bmp")
                    % shp_file_name.stem().string()
                    % tile_index++
        ).str();
    };

    if (vm["help"].as<bool>()) {
        std::cout << usage << std::endl;
        return 0;
    }

    if (vm.count("input-map-file") == 0) {
        std::cerr << "MAP file path is missing!" << std::endl
            << usage
            << std::endl;
        return 1;
    }

    if (vm.count("input-icn-file") == 0) {
        std::cerr << "ICN file path is missing!" << std::endl
            << usage
            << std::endl;
        return 1;
    }

    if (vm.count("input-pal-file") == 0) {
        std::cerr << "PAL file path is missing!" << std::endl
            << usage
            << std::endl;
        return 1;
    }

    const auto pal = nr::dune2::Palette::load(vm["input-pal-file"].as<fs::path>());
    if (!pal) {
        std::cerr << "Cannot load PAL file!" << std::endl;
        return 1;
    }

    const auto icn = nr::dune2::ICN::load(
        vm["input-map-file"].as<fs::path>(),
        vm["input-icn-file"].as<fs::path>(),
        *pal
    );
    if (!icn) {
        std::cerr << "Cannot load ICN file!" << std::endl;
        return 1;
    }

    for (auto &&icon: *icn) {
        nr::dune2::BMP bmp(icon.getWidth(), icon.getHeight());
        bmp.drawSurface(0, 0, icon);
        bmp.store(vm["output-dir"].as<fs::path>()/output_file_name());
    }

    return 0;
}

#include <Dune2/bmp.hpp>
#include <Dune2/iconset.hpp>
#include <Dune2/tileset.hpp>

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
            "icons-tiles-map,m",
            po::value<fs::path>(),
            "Provide icons tiles mapping file."
        ) (
            "output-dir,d",
            po::value<fs::path>()->default_value(fs::current_path()),
            "Set output directory."
        );

    po::options_description hidden;
    hidden.add_options()
        ("input-pal-file", po::value<fs::path>(), "PAL file path.")
        ("input-icn-file", po::value<fs::path>(), "ICN file path.");

    po::positional_options_description args;
    args
        .add("input-pal-file", 1)
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

    const auto tileset = nr::dune2::Tileset::load(vm["input-icn-file"].as<fs::path>());
    if (!tileset) {
        std::cerr << "Cannot load ICN file!" << std::endl;
        return 1;
    }

    if (vm.count("icons-tiles-map") == 0) {
        for (auto &&surface: *tileset) {
            nr::dune2::BMP bmp(surface.getWidth(), surface.getHeight());
            bmp.drawSurface(0, 0, surface, *pal);
            bmp.store(vm["output-dir"].as<fs::path>()/output_file_name());
        }
        return 0;
    }
    
    const auto iconset = nr::dune2::Iconset::load(vm["icons-tiles-map"].as<fs::path>(), *tileset);
    if (!iconset) {
        std::cerr << "Cannot load MAP file!" << std::endl;
        return 1;
    } else {
        for (auto &&surface: *iconset) {
            nr::dune2::BMP bmp(surface.getWidth(), surface.getHeight());
            bmp.drawSurface(0, 0, surface, *pal);
            bmp.store(vm["output-dir"].as<fs::path>()/output_file_name());
        }
    }

    return 0;
}

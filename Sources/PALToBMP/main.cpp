#include <Dune2/bmp.hpp>
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
        ("input-pal-file", po::value<fs::path>(), "PAL filepath.");

    po::positional_options_description args;
    args
        .add("input-pal-file", 1);

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
            << "Usage: " << fs::path(argv[0]).stem().string() << " [OPTIONS] pal-file"
            << "\n\n"
            << options
            << std::endl;
    };

    const auto verbose = [&]() {
        return vm["verbose"].as<bool>();
    };

    const auto outputBMPFile = [&]() {
        const auto output_dir = vm["output-dir"].as<fs::path>();
        const auto pal_file_name = vm["input-pal-file"].as<fs::path>();

        return output_dir/(pal_file_name.stem().string() + ".bmp");
    };

    if (vm["help"].as<bool>()) {
        std::cout << usage << std::endl;
        return 0;
    }

    if (vm.count("input-pal-file") == 0) {
        std::cerr << "PAL file is missing!" << std::endl
            << usage
            << std::endl;
        return 1;
    }

    const auto palette = nr::dune2::Palette::load(vm["input-pal-file"].as<fs::path>());
    if (!palette) {
        std::cerr << "Cannot load PAL file!" << std::endl;
        return 1;
    }

    nr::dune2::BMP bmp(256, 256);
    for (auto i = 0; i < 256; ++i) {
        const auto row = i/16;
        const auto col = i%16;
        bmp.fillRect(col*16, row*16, 16, 16, (*palette)[i]);
    }

    bmp.store(outputBMPFile());

    return 0;
}

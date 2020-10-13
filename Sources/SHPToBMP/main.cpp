#include <Dune2/bmp.hpp>
#include <Dune2/io.hpp>
#include <Dune2/palette.hpp>
#include <Dune2/shp.hpp>

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
        ("input-pal-file", po::value<fs::path>(), "PAL filepath.")
        ("input-shp-file", po::value<fs::path>(), "SHP filepath.");

    po::positional_options_description args;
    args
        .add("input-pal-file", 1)
        .add("input-shp-file", 1);

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
            << "Usage: " << fs::path(argv[0]).stem().string() << " [OPTIONS] pal-file shp-file"
            << "\n\n"
            << options
            << std::endl;
    };

    const auto verbose = [&]() {
        return vm["verbose"].as<bool>();
    };

    auto output_file_name = [&vm, frame_index = 1]() mutable {
        using boost::format;
        const auto shp_file_name = vm["input-shp-file"].as<fs::path>();
        return (format("%1%#%2%.bmp")
                    % shp_file_name.stem().string()
                    % frame_index++
        ).str();
    };

    if (vm["help"].as<bool>()) {
        std::cout << usage << std::endl;
        return 0;
    }

    if (vm.count("input-shp-file") == 0) {
        std::cerr << "SHP file is missing!" << std::endl
            << usage
            << std::endl;
        return 1;
    }

    if (vm.count("input-pal-file") == 0) {
        std::cerr << "PAL file is missing!" << std::endl
            << usage
            << std::endl;
        return 1;
    }

    const auto shp = nr::dune2::SHP::load(vm["input-shp-file"].as<fs::path>());
    if (!shp) {
        std::cerr << "Cannot load SHP file!" << std::endl;
        return 1;
    }

    const auto palette = nr::dune2::Palette::load(vm["input-pal-file"].as<fs::path>());
    if (!palette) {
        std::cerr << "Cannot load PAL file!" << std::endl;
        return 1;
    }

    for (auto &&frame: *shp) {
        nr::dune2::BMP bmp(frame.width, frame.height);
        for (auto i = 0; i < frame.data.size(); ++i) {
            const auto row = i/frame.width;
            const auto col = i%frame.height;
            const auto color_index = frame[i];
            if (color_index > 0) {
                bmp.putPixel(col, row, (*palette)[color_index]);
            } else {
                bmp.putPixel(col, row, nr::dune2::Color{
                    0x0,
                    0xff,
                    0x0,
                });
            }
        }
        bmp.store(vm["output-dir"].as<fs::path>()/output_file_name());
    }

    return 0;
}

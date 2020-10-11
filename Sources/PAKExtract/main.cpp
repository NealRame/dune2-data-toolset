#include <Dune2/io.hpp>
#include <Dune2/pak.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

#include <boost/program_options.hpp>

namespace po = boost::program_options;
namespace fs = std::filesystem;

int
main(int argc, char const *argv[]) {
    po::options_description options("OPTIONS");
    options.add_options()
        ("help,h", po::bool_switch(), "Display help message and exit.")
        ("list,l", po::bool_switch(), "List file in the PAK.")
        ("verbose,v", po::bool_switch(), "Enable verbosity.")
        ("output-dir,d", po::value<fs::path>(), "Set output directory.");

    po::options_description hidden;
    hidden.add_options()
        ("input-file,i", po::value<fs::path>(), "Set input file.");

    po::positional_options_description args;
    args.add("input-file", 1);

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
        return out << "Usage: " << fs::path(argv[0]).stem().string() << " [OPTIONS] input-pak-file"
            << "\n\n"
            << options
            << std::endl;
    };

    const auto verbose = [&]() {
        return vm["verbose"].as<bool>();
    };

    const auto outputDir = [&]() {
        return vm.count("output-dir")
            ? vm["output-dir"].as<fs::path>()
            : vm["input-file"].as<fs::path>().stem();
    };

    const auto list = [&](const nr::dune2::PAK &pak) {
        for (const auto &entry: pak) {
            std::cout << entry.name << " " << entry.size << std::endl;
        }
    };

    const auto extract = [&](const nr::dune2::PAK &pak) {
        fs::create_directories(outputDir());
        for (const auto &entry: pak) {
            auto out = std::fstream(
                outputDir()/entry.name,
                std::ios::binary | std::ios::out | std::ios::trunc
            );

            if(verbose()) {
                std::cerr << "extracting " << entry.name << " ... " << std::endl;
            }

            out.write(entry.read().data(), entry.size);
        }
    };

    if (vm["help"].as<bool>()) {
        std::cout << usage << std::endl;
        return 0;
    }

    if(vm.count("input-file")) {
        const auto filepath = vm["input-file"].as<fs::path>();
        const auto pak = nr::dune2::PAK::load(filepath.string());

        if (pak) {
            if (vm["list"].as<bool>()) {
                list(*pak);
            } else {
                extract(*pak);
            }
        }
    } else {
        std::cerr
            << "Missing input-pak-file!"
            << std::endl
            << usage
            << std::endl;
        return 1;
    }

    return 0;
}

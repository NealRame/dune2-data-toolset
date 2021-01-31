#pragma once

#include <Dune2/resources.hpp>

#include <CLI/CLI.hpp>

#include <filesystem>

namespace fs = std::filesystem;

using CLI::App;

struct AppState {
    class Resource {
        std::filesystem::path filepath_;
        nr::dune2::Resource rc_;

    public:
        Resource(const fs::path &);
        ~Resource();

        Resource(const Resource &) = delete;
        Resource &operator=(const Resource &) = delete;

        nr::dune2::Resource *operator->();
        const nr::dune2::Resource *operator->() const;

        nr::dune2::Resource &operator*();
        const nr::dune2::Resource &operator*() const;
    };

    unsigned int verbose{0};
    fs::path dune2RCPath{fs::current_path()/"dune2.rc"};

    Resource resource();
};
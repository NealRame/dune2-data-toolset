#pragma once

#include <Dune2/io.hpp>
#include <Dune2/palette.hpp>
#include <Dune2/icon_set.hpp>
#include <Dune2/image.hpp>
#include <Dune2/image_set.hpp>

#include <CLI/CLI.hpp>

#include <rapidjson/document.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>

#include <filesystem>

using CLI::App;

namespace nr {

struct AppState {
    unsigned int verbose{0};
};


bool filepathMatch(const std::filesystem::path &, const std::string extension);

template <typename T>
void flushJSON(const rapidjson::Document &json, bool pretty, T &output) {
    using rapidjson::OStreamWrapper;
    using PrettyWriter = rapidjson::PrettyWriter<OStreamWrapper>;
    using Writer = rapidjson::Writer<OStreamWrapper>;

    OStreamWrapper osw(output);

    if (pretty) {
        PrettyWriter writer(osw);
        json.Accept(writer);
    } else {
        Writer writer(osw);
        json.Accept(writer);
    }
}

template <typename T>
void load(T &data, const std::filesystem::path &);

template <>
void load<dune2::Palette>(dune2::Palette &, const std::filesystem::path &);

template <>
void load<dune2::ImageSet>(dune2::ImageSet &, const std::filesystem::path &);

template <>
void load<dune2::IconSet>(dune2::IconSet &, const std::filesystem::path &);

} // namespace nr

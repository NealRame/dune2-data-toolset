#include "resources.hpp"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <fstream>
#include <iostream>

namespace nr::dune2 {

void export_palettes(
    const Resource &rc,
    const std::filesystem::path &output_dir
) {
    using rapidjson::StringBuffer;
    using rapidjson::Writer;

    StringBuffer s;
    Writer writer(s);

    writer.StartObject();
    for (const auto &name: rc.getPaletteList()) {
        writer.Key(name.c_str());
        writer.StartArray();
        for (const auto &color: rc.getPalette(name)) {
            writer.StartArray();
                writer.Uint(color.red);
                writer.Uint(color.green);
                writer.Uint(color.blue);
            writer.EndArray();
        }
        writer.EndArray();
    }
    writer.EndObject();

    std::ofstream(output_dir/"palette.json")
        << s.GetString()
        << std::endl;
}

void
Resource::json_export(const std::filesystem::path &output_dir) const {
    // First export palette.json
    export_palettes(*this, output_dir);
}

} // namespace nr::dune2

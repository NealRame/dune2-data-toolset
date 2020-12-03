#include "pak.hpp"
#include "io.hpp"

#include <fstream>
#include <istream>
#include <utility>

namespace {

using PAKRawEntry = std::pair<size_t, std::string>;

} // namespace

namespace std {

std::istream &
operator>>(std::istream &in, PAKRawEntry &entry) {
    using nr::dune2::io::readLEInteger;
    using nr::dune2::io::readString;
    if (in) {
        const auto offset = readLEInteger<4, PAKRawEntry::first_type>(in);
        if (offset > 0) {
            entry = std::make_pair(offset, readString(in));
        } else {
            entry = std::make_pair(
                static_cast<size_t>(in.seekg(0, std::ios::end).tellg()),
                ""
            );
        }
    }
    return in;
}
} // namespace std

namespace nr::dune2 {

///////////////////////////////////////////////////////////////////////////////
// PAK

void
PAK::load(const std::string &filepath) {
    std::ifstream input;

    input.open(filepath, std::ios::binary);

    std::vector<PAKRawEntry> raw_entries;
    std::copy(
        std::istream_iterator<PAKRawEntry>(input),
        std::istream_iterator<PAKRawEntry>(),
        std::back_inserter(raw_entries)
    );

    std::transform(
        std::begin(raw_entries),
        std::end(raw_entries) - 1,
        std::begin(raw_entries) + 1,
        std::back_inserter(entries_),
        [&](const auto &raw_entry1, const auto &raw_entry2) {
            return Entry{
                .offset   = raw_entry1.first,
                .size     = raw_entry2.first - raw_entry1.first,
                .name     = raw_entry1.second,
                .filepath = std::make_shared<std::string>(filepath)
            };
        }
    );
}

PAK::const_iterator
PAK::begin() const {
    return entries_.begin();
}

PAK::const_iterator
PAK::end() const {
    return entries_.end();
}

PAK::const_iterator
PAK::cbegin() const {
    return begin();
}

PAK::const_iterator
PAK::cend() const {
    return end();
}

///////////////////////////////////////////////////////////////////////////////
// PAK::Entry

std::string
PAK::Entry::read() const {
    std::string buf(size, 0);
    std::ifstream input(*filepath, std::ifstream::binary);
    input.seekg(offset);
    input.read(buf.data(), buf.size());
    return buf;
}

} // namespace nr::dune2

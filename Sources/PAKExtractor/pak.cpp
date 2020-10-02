#include "pak.hpp"

#include <Dune2/io.hpp>

#include <fstream>
#include <istream>
#include <optional>
#include <utility>

namespace {

using PAKRawEntry = std::pair<std::size_t, std::string>;

} // namespace

namespace std {

std::istream &
operator>>(std::istream &in, PAKRawEntry &entry) {
    using nr::dune2::io::readInteger;
    using nr::dune2::io::readString;
    if (in) {
        const auto offset = readInteger<4, PAKRawEntry::first_type>(in);
        if (offset > 0) {
            entry = std::make_pair(offset, readString(in));
        } else {
            entry = std::make_pair(
                static_cast<std::size_t>(in.seekg(0, std::ios::end).tellg()),
                ""
            );
        }
    }
    return in;
}

std::ostream &
operator<<(std::ostream &out, const nr::dune2::PAK::Entry &entry) {
    auto buf = entry.buffer();
    out << &buf;
    return out;
}
} // namespace std

namespace nr::dune2 {

///////////////////////////////////////////////////////////////////////////////
// PAK

std::optional<PAK>
PAK::load(const std::string &filepath) {
    std::fstream input(filepath, std::fstream::in|std::fstream::binary);

    if (!input) return std::nullopt;

    std::vector<PAKRawEntry> raw_entries;
    std::copy(
        std::istream_iterator<PAKRawEntry>(input),
        std::istream_iterator<PAKRawEntry>(),
        std::back_inserter(raw_entries)
    );

    auto pak = std::make_optional<PAK>();
    std::transform(
        std::begin(raw_entries),
        std::end(raw_entries) - 1,
        std::begin(raw_entries) + 1,
        std::back_inserter(pak->entries_),
        [&](const auto &raw_entry1, const auto &raw_entry2) {
            return Entry{
                .offset   = raw_entry1.first,
                .size     = raw_entry2.first - raw_entry1.first,
                .name     = raw_entry1.second,
                .filepath = std::make_shared<std::string>(filepath)
            };
        }
    );

    return pak;
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
// PAK::EntryBuffer

PAK::EntryBuffer::EntryBuffer(const Entry &entry)
    : offset_{entry.offset}
    , size_{entry.size}
    , pos_{0}
    , ch_{0}
    , input_(*entry.filepath, std::fstream::in|std::fstream::binary) {
}

std::streambuf::int_type
PAK::EntryBuffer::underflow() {
    const auto _EOF = std::char_traits<char>::eof();
    if (input_ && pos_ < size_) {
        input_.clear();
        input_.seekg(offset_ + pos_);
        if (input_) {
            ch_ = input_.get();
            pos_ += 1;
        }
    } else {
        ch_ = _EOF;
    }
    setg(&ch_, &ch_, &ch_ + 1);
    return ch_;
}

///////////////////////////////////////////////////////////////////////////////
// PAK::Entry

PAK::EntryBuffer
PAK::Entry::buffer() const {
    EntryBuffer buf(*this);
    return buf;
}

} // namespace nr::dune2

#include "shp.hpp"
#include "io.hpp"

#include <bitset>
#include <fstream>

namespace nr::dune2 {

namespace {

SHP::Version
shp_read_version(std::fstream &input) {
    io::IPosOffsetGuard _(input);
    input.seekg(4);
    return nr::dune2::io::readLEInteger<2>(input) != 0
        ? SHP::v100
        : SHP::v107;
}

template<SHP::Version V>
std::istream::pos_type
shp_read_frame_offset(std::istream &input) {
    if constexpr(V == SHP::v100) {
        return io::readLEInteger<2, std::uint16_t>(input);
    } else {
        return io::readLEInteger<4, std::uint32_t>(input) + 2;
    }
}

std::vector<std::istream::pos_type>
shp_read_frame_offsets(std::istream &input, SHP::Version version) {
    std::vector<std::istream::pos_type> offsets;
    const auto frame_count = io::readLEInteger<2, unsigned int>(input);
    std::generate_n(
        std::back_inserter(offsets),
        frame_count,
        [&]() {
            return version == SHP::v100
                ? shp_read_frame_offset<SHP::v100>(input)
                : shp_read_frame_offset<SHP::v107>(input);
        }
    );
    return offsets;
}

void
shp_read_data(std::istream &input, std::size_t count, std::vector<uint8_t> &dst) {
    const auto pos = dst.size();
    dst.resize(dst.size() + count);
    input.read(reinterpret_cast<char *>(dst.data()) + pos, count);
}

void
shp_lcw_deflate(std::istream &input, std::size_t size, std::vector<uint8_t> &dst) {
    const auto read_byte = [&]() { return io::readLEInteger<1, uint8_t>(input); };
    const auto read_word = [&]() { return io::readLEInteger<2, uint16_t>(input); };
    const auto copy_block = [&](auto count, auto pos, bool relative) {
        std::copy_n(
            relative
                ? dst.end() - pos
                : dst.begin() + pos,
            count,
            std::back_inserter(dst)
        );
    };

    // Ignore first byte if it is the relative mode flag
    const auto relative = input.peek() == 0;
    if (relative) input.ignore(1);

    // LCW data should end with a 0x80 byte.
    // We also have a given amount of bytes we may read before ending the 
    // deflate process.
    // Well-formed LCW data should match flag 0x80 with the maximum number of
    // bytes of data to read reached.
    const auto end_pos = std::size_t(input.tellg()) + size;

    for (uint8_t cmd; (cmd = read_byte()) != 0x80 && input.tellg() < end_pos;) {
        if ((cmd & 0xc0) == 0x80) {
            // command 1: short copy
            // 0b10cccccc
            const auto count = cmd & 0x3f;
            shp_read_data(input, count, dst);
        } else if ((cmd & 0x80) == 0) {
            // command 2: existing block relative copy
            // 0b0cccpppp p
            const auto count = ((cmd & 0x70)>>4) + 3;
            const auto pos   = ((cmd & 0x0f)<<8) | read_byte();
            if (pos == 1) {
                dst.insert(dst.end(), count, dst.back());
            } else {
                copy_block(count, pos, true);
            }
        } else if (cmd == 0xfe) {
            // command 4: repeat value
            // 0b11111110 c c v
            const auto count = read_word();
            const auto value = read_byte();
            dst.insert(dst.end(), count, value);
        } else if (cmd == 0xff) {
            // command 5: existing block long copy
            // 0b11111111 c c p p
            const auto count = read_word();
            const auto pos   = read_word();
            copy_block(count, pos, relative);
        } else {
            // command 3: existing block medium-length copy
            // 0b11cccccc p p
            const auto count = (cmd & 0x3f) + 3;
            const auto pos   = read_word();
            copy_block(count, pos, relative);
        }
    }
}

} // namespace

std::optional<SHP>
SHP::load(const std::string &filepath, const Palette &palette) {
    std::fstream input(filepath, std::ios::binary|std::ios::in);

    auto shp = std::make_optional<SHP>();

    const auto version = shp_read_version(input);
    const auto offsets = shp_read_frame_offsets(input, version);

    std::transform(
        offsets.begin(),
        offsets.end(),
        std::back_inserter(shp->frames_),
        [&](const auto pos) {
            return std::move(Frame(input, pos, palette));
        }
    );

    return shp;
}

SHP::const_iterator
SHP::begin() const {
    return frames_.begin();
}

SHP::const_iterator
SHP::end() const {
    return frames_.end();
}

SHP::const_iterator
SHP::cbegin() const {
    return begin();
}

SHP::const_iterator
SHP::cend() const {
    return end();
}

struct SHP::Frame::impl {
    const Palette &palette;
    std::size_t width;
    std::size_t height;
    std::vector<uint8_t> remapTable;
    std::vector<uint8_t> data;
    impl(const Palette &palette) : palette{palette} { }
};

SHP::Frame::Frame(
    std::istream &input,
    std::istream::pos_type pos,
    const Palette &palette)
    : d{std::make_unique<Frame::impl>(palette)} {
    input.seekg(pos);

    std::bitset<16> frame_flags(io::readLEInteger<2>(input));

    static const auto HasRemapTable = 0u;
    static const auto NoLCW = 1u;
    static const auto CustomSizeRemap = 2u;

    input.ignore(1);

    d->width = io::readLEInteger<2, std::size_t>(input);
    d->height = io::readLEInteger<1, std::size_t>(input);
    d->data.reserve(d->width*d->height);

    const auto frame_size = io::readLEInteger<2, std::size_t>(input);
    const auto rle_data_size = io::readLEInteger<2, std::size_t>(input);

    if (frame_flags[HasRemapTable]) {                        // HasRemapTable is set
        const auto remap_size = frame_flags[CustomSizeRemap] // CustomSizeRemap is set
            ? io::readLEInteger<1, std::size_t>(input)
            : 16;
        
        for (auto i = 0; i < remap_size; ++i) {
            d->remapTable.push_back(io::readLEInteger<1, std::uint8_t>(input));
        }
    }

    const auto frame_data_size = frame_size - (input.tellg() - pos);

    std::vector<uint8_t> rle_data;
    rle_data.reserve(rle_data_size);
    if (frame_flags[1]) {
        shp_read_data(input, frame_data_size, rle_data);
    } else {
        shp_lcw_deflate(input, frame_data_size, rle_data);
    }

    auto it = rle_data.begin();
    while (it != rle_data.end()) {
        const auto n = it - rle_data.begin();
        const auto value = *it++;
        const auto count = (value == 0 ? *it++ : 1);
        d->data.insert(d->data.end(), count, value);
    }
}

SHP::Frame::Frame(Frame &&rhs) {
    *this = std::move(rhs);
}

SHP::Frame &
SHP::Frame::operator=(Frame &&rhs) {
    d = std::move(rhs.d);
    return *this;
}

SHP::Frame::~Frame() {
}

std::size_t
SHP::Frame::getWidth() const {
    return d->width;
}

std::size_t
SHP::Frame::getHeight() const {
    return d->height;
}

Color
SHP::Frame::getPixel(std::size_t x, std::size_t y) const {
    assert(x < getWidth());
    assert(y < getHeight());
    const auto index = d->data[y*getWidth() + x];
    return d->palette[d->remapTable.size() > 0
        ? d->remapTable[index]
        : index
    ];
}

} // namespace nr::dune2

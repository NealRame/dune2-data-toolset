#pragma once

#include <Dune2/bswap.hpp>

#include <array>
#include <functional>
#include <istream>
#include <string_view>
#include <vector>

namespace nr::dune2::io {

class IPosOffsetGuard {
    std::istream &input_;
    std::istream::pos_type pos_;

public:
    IPosOffsetGuard(std::istream &);
    ~IPosOffsetGuard();
};

/// readLEInteger
/// generic function to read a little endian integer on a input stream
template<int N, typename IntType = uint32_t>
IntType
readLEInteger(std::istream &input) {
    static_assert(std::is_integral<IntType>::value, "Integral type required");
    static_assert(sizeof(IntType) >= N, "Integral type size too small");

    IntType value = IntType{0};
    input.read(((char *)&value), N);

    return value;
}

/// readBEInteger
/// generic function to read a big endian integer on a input stream
template<int N, typename IntType = uint32_t>
IntType
readBEInteger(std::istream &input) {
    static_assert(std::is_integral<IntType>::value, "Integral type required");
    static_assert(sizeof(IntType) >= N, "Integral type size too small");

    IntType value = IntType{0};
    input.read(((char *)&value), N);

    return byte_swap<IntType>::swap(value);
}

struct InputCheckFailedError : public std::exception {
};

template<typename ValueType>
void
check(std::istream &input, ValueType v) {
    constexpr auto value = v();
    constexpr auto N = value.size();
    std::array<char, N> buf;
    try {
        input.read(buf.data(), N);
    } catch (...) {
        throw InputCheckFailedError();
    }
    if (input.gcount() != N || std::string_view(buf.data(), N) != value) {
        throw InputCheckFailedError();
    }
}

// readData
// read a given amount of data from the input stream and return it in a vector
template<typename T = std::uint8_t>
std::vector<T>
readData(std::istream &input, size_t count) {
    static_assert(std::is_trivial_v<T>, "Trivial type required");

    std::vector<T> buffer(count);
    const auto data_size = count*sizeof(T);

    input.read(reinterpret_cast<char *>(buffer.data()), data_size);

    return buffer;
}

std::vector<uint8_t> readLCWData(std::istream &, size_t deflated_size, size_t inflated_size);

std::string readAll(std::istream &);
std::string readString(std::istream &);
std::string readString(std::istream &, size_t);

class OPosOffsetGuard {
    std::ostream &output_;
    std::ostream::pos_type pos_;

public:
    OPosOffsetGuard(std::ostream &);
    ~OPosOffsetGuard();
};

// writeInteger
template<int N, typename IntType>
void
writeInteger(std::ostream &output, IntType value) {
    static_assert(std::is_integral<IntType>::value, "Integral type required");
    static_assert(N <= sizeof(uintmax_t), "N is to big");

    std::conditional_t<
        std::is_unsigned_v<IntType>,
        uintmax_t,
        intmax_t
    > v = value;

    output.write(reinterpret_cast<char *>(&v), N);
}

} // namespace nr::dune2

std::ostream & operator<<(std::ostream &, const std::function<std::ostream &(std::ostream &)> &);
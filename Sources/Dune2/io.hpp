#pragma once

#include <functional>
#include <istream>
#include <optional>

namespace nr::dune2::io {

/// readInteger
/// generic function to read an integer on a input stream
template<int N, typename IntType = uint32_t>
IntType
readInteger(std::istream &input) {
    static_assert(std::is_integral<IntType>::value, "Integral type required");
    static_assert(sizeof(IntType) >= N, "Integral type size too small");

    IntType value = IntType{0};
    input.read(((char *)&value), N);

    return value;
}

// readData
// read a given amount of data from the input stream and return it in a vector
template<typename T = std::uint8_t>
std::vector<T>
readData(std::istream &input, std::size_t count) {
    static_assert(std::is_trivial_v<T>, "Trivial type required");

    std::vector<T> buffer(count);
    const auto data_size = count*sizeof(T);

    input.read(reinterpret_cast<char *>(buffer.data()), data_size);

    return buffer;
}

std::string readString(std::istream &);
} // namespace nr::dune2

std::ostream & operator<<(std::ostream &, const std::function<std::ostream &(std::ostream &)> &);
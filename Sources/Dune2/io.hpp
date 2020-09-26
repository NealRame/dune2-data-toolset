#pragma once

#include <functional>
#include <istream>
#include <optional>

namespace nr::dune2::io {

/// readInteger
/// generic function to read an integer on a input stream
template<int N, typename IntType = uint32_t>
std::optional<IntType>
readInteger(std::istream &in) {
    static_assert(std::is_integral<IntType>::value, "Integral type required");
    static_assert(sizeof(IntType) >= N, "Integral type size too small");

    IntType value = IntType{0};
    in.read(((char *)&value), N);

    return in.gcount() == N
        ? std::make_optional<IntType>(value)
        : std::nullopt;
}

std::string readString(std::istream &);
} // namespace nr::dune2

std::ostream & operator<<(std::ostream &, const std::function<std::ostream &(std::ostream &)> &);
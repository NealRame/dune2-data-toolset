#pragma once

#include <cstdint>

namespace nr::dune2 {

template <typename T>
struct byte_swap {};

template <>
struct byte_swap<uint8_t> {
    static uint8_t swap(uint8_t v) {
        return v;
    }
};
template <>
struct byte_swap<int8_t> {
    static int8_t swap(int8_t v) {
        return v;
    }
};

template <>
struct byte_swap<uint16_t> {
    static uint16_t swap(uint16_t v) {
        return __builtin_bswap16(v);
    }
};
template <>
struct byte_swap<int16_t> {
    static int16_t swap(int16_t v) {
        return __builtin_bswap16(v);
    }
};

template <>
struct byte_swap<uint32_t> {
    static uint32_t swap(uint32_t v) {
        return __builtin_bswap32(v);
    }
};
template <>
struct byte_swap<int32_t> {
    static int32_t swap(int32_t v) {
        return __builtin_bswap32(v);
    }
};

template <>
struct byte_swap<uint64_t> {
    static uint64_t swap(uint64_t v) {
        return __builtin_bswap64(v);
    }
};
template <>
struct byte_swap<int64_t> {
    static int64_t swap(int64_t v) {
        return __builtin_bswap32(v);
    }
};

} // namespace nr::dune2

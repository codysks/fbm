#pragma once

#include "bmconcepts.hpp"
#include <cstdint>

template <SizeByte T>
constexpr uint16_t unpackle8(T const* data) {
    uint16_t ret = data[0];
    return ret;
}

template <SizeByte T>
constexpr uint16_t unpackle16(T const* data) {
    uint16_t ret = data[1] << 8 | data[0];
    return ret;
}

template <SizeByte T>
constexpr uint32_t unpackle32(T const* data) {
    uint32_t ret = data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0];
    return ret;
}

template <SizeByte T>
constexpr uint64_t unpackle64(T const* data) {
    uint64_t ret = data[7] << 56 | data[6] << 48 | data[5] << 40 | data[4] << 32 |
                   data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0];
    return ret;
}

template <SizeByte T>
constexpr uint16_t unpack8(T const* data) {
    uint16_t ret = data[0];
    return ret;
}

template <SizeByte T>
constexpr uint16_t unpack16(T const* data) {
    uint16_t ret = data[0] << 8 | data[1];
    return ret;
}

template <SizeByte T>
constexpr uint32_t unpack32(T const* data) {
    uint32_t ret = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
    return ret;
}

template <SizeByte T>
constexpr uint64_t unpack64(T const* data) {
    uint64_t ret = data[0] << 56 | data[1] << 48 | data[2] << 40 | data[3] << 32 |
                   data[4] << 24 | data[5] << 16 | data[6] << 8 | data[7];
    return ret;
}
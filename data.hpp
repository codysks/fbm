#pragma once

#include <string>
#include <iostream>
#include <cstddef>
#include <cstdint>
#include <array>
#include <format>
#include <stdexcept>
#include <cstring>
#include <cassert>


template <typename T, bool is_little_endian = false>
class Data {
    static_assert(std::is_unsigned_v<T>);
public:
    inline Data(T const value) : value_(value) {}
    inline Data(void) {}
    template <typename U> size_t unpack(U const* data);
    static consteval size_t length(void) noexcept { return sizeof(T); }
    operator T(void) { return value_; }
    operator std::string(void) { return std::to_string(value_); }
    T get(void) const { return value_; }
private:
    T value_;
};

template <typename T, bool is_little_endian>
template <typename U>
size_t Data<T, is_little_endian>::unpack(U const* data) {
    value_ = 0;
    static_assert(sizeof(U) == 1, "Only accepts byte data");
    constexpr auto size_of_underlying_type = length();
    if constexpr(is_little_endian) {
        for (size_t i = size_of_underlying_type; i != 0; --i) {
            value_ <<= 8;
            value_ |= std::bit_cast<unsigned char>(data[i - 1]);
        }
    } else {
        for (size_t i = 0; i != size_of_underlying_type; ++i) {
            value_ <<= 8;
            value_ |= std::bit_cast<unsigned char>(data[i]);
        }
    }
    return size_of_underlying_type;
}

template <typename T, bool is_little_endian>
struct std::formatter<Data<T, is_little_endian>, char> {
    bool hex = false;
    constexpr auto parse(std::format_parse_context& ctx) {
        auto it = ctx.begin();
        for (; *it != '}'; ++it) {
            switch (*it) {
                case 'x': hex = true; break;
                default: throw std::format_error("Invalid format");
            }
        }
        return it;
    }
    auto format(Data<T, is_little_endian> const& data, std::format_context& ctx) const {
        if (hex) {
            return std::format_to(ctx.out(), "0x{:x}", data.get());
        } else {
            return std::format_to(ctx.out(), "{}", data.get());

        }
    }
};

#pragma once

#include <cstdint>
#include "cppunpack.hpp"
#include <ostream>
#include <format>

template <typename UnderlyingT>
class UnsignedWrapper {
public:
    constexpr size_t size_of(void) { return sizeof(UnderlyingT); }
    constexpr UnsignedWrapper(void) = default;
    constexpr UnsignedWrapper(UnderlyingT const v) : value_(v) { }
	constexpr operator UnderlyingT(void) const { return value_; }
    constexpr UnsignedWrapper(UnsignedWrapper const& v) : value_(v.get()) { }

    UnderlyingT get(void) const { return value_; }
    void set(UnderlyingT v) { value_ = v; }

    // --- Comparison ---
    bool operator==(UnsignedWrapper const& other) const = default;
    auto operator<=>(UnsignedWrapper const& other) const = default;

    // --- Arithmetic (Unary) ---
    UnsignedWrapper& operator++(void) { ++value_; return *this; }
    UnsignedWrapper operator++(int) { UnsignedWrapper temp = *this; ++value_; return temp; }
    UnsignedWrapper& operator--(void) { --value_; return *this; }
    UnsignedWrapper operator--(int) { UnsignedWrapper temp = *this; --value_; return temp; }

    // --- Arithmetic (Assignment) ---
    UnsignedWrapper& operator+=(UnsignedWrapper const& other) { value_ += other.value_; return *this; }
    UnsignedWrapper& operator-=(UnsignedWrapper const& other) { value_ -= other.value_; return *this; }
    UnsignedWrapper& operator*=(UnsignedWrapper const& other) { value_ *= other.value_; return *this; }
    UnsignedWrapper& operator/=(UnsignedWrapper const& other) { value_ /= other.value_; return *this; }
    UnsignedWrapper& operator%=(UnsignedWrapper const& other) { value_ %= other.value_; return *this; }

    // --- Arithmetic (Binary) ---
    friend UnsignedWrapper operator+(UnsignedWrapper lhs, UnsignedWrapper const& rhs) { lhs += rhs; return lhs; }
    friend UnsignedWrapper operator-(UnsignedWrapper lhs, UnsignedWrapper const& rhs) { lhs -= rhs; return lhs; }
    friend UnsignedWrapper operator*(UnsignedWrapper lhs, UnsignedWrapper const& rhs) { lhs *= rhs; return lhs; }
    friend UnsignedWrapper operator/(UnsignedWrapper lhs, UnsignedWrapper const& rhs) { lhs /= rhs; return lhs; }
    friend UnsignedWrapper operator%(UnsignedWrapper lhs, UnsignedWrapper const& rhs) { lhs %= rhs; return lhs; }

    // --- Bitwise (Unary) ---
    UnsignedWrapper operator~(void) const { return UnsignedWrapper(~value_); }

    // --- Bitwise (Assignment) ---
    UnsignedWrapper& operator&=(UnsignedWrapper const& other) { value_ &= other.value_; return *this; }
    UnsignedWrapper& operator|=(UnsignedWrapper const& other) { value_ |= other.value_; return *this; }
    UnsignedWrapper& operator^=(UnsignedWrapper const& other) { value_ ^= other.value_; return *this; }
    UnsignedWrapper& operator<<=(unsigned shift) { value_ <<= shift; return *this; }
    UnsignedWrapper& operator>>=(unsigned shift) { value_ >>= shift; return *this; }

    // --- Bitwise (Binary) ---
    friend UnsignedWrapper operator&(UnsignedWrapper lhs, UnsignedWrapper const& rhs) { lhs &= rhs; return lhs; }
    friend UnsignedWrapper operator|(UnsignedWrapper lhs, UnsignedWrapper const& rhs) { lhs |= rhs; return lhs; }
    friend UnsignedWrapper operator^(UnsignedWrapper lhs, UnsignedWrapper const& rhs) { lhs ^= rhs; return lhs; }
    friend UnsignedWrapper operator<<(UnsignedWrapper lhs, unsigned shift) { lhs <<= shift; return lhs; }
    friend UnsignedWrapper operator>>(UnsignedWrapper lhs, unsigned shift) { lhs >>= shift; return lhs; }

    // --- Symmetrical Arithmetic with UnderlyingT ---
    
    // Addition
    friend constexpr UnsignedWrapper operator+(UnsignedWrapper lhs, UnderlyingT rhs) { return lhs.value_ + rhs; }
    friend constexpr UnsignedWrapper operator+(UnderlyingT lhs, UnsignedWrapper rhs) { return lhs + rhs.value_; }

    // Subtraction
    friend constexpr UnsignedWrapper operator-(UnsignedWrapper lhs, UnderlyingT rhs) { return lhs.value_ - rhs; }
    friend constexpr UnsignedWrapper operator-(UnderlyingT lhs, UnsignedWrapper rhs) { return lhs - rhs.value_; }

    // Multiplication
    friend constexpr UnsignedWrapper operator*(UnsignedWrapper lhs, UnderlyingT rhs) { return lhs.value_ * rhs; }
    friend constexpr UnsignedWrapper operator*(UnderlyingT lhs, UnsignedWrapper rhs) { return lhs * rhs.value_; }

    // Division
    friend constexpr UnsignedWrapper operator/(UnsignedWrapper lhs, UnderlyingT rhs) { return lhs.value_ / rhs; }
    friend constexpr UnsignedWrapper operator/(UnderlyingT lhs, UnsignedWrapper rhs) { return lhs / rhs.value_; }

    // Modulo
    friend constexpr UnsignedWrapper operator%(UnsignedWrapper lhs, UnderlyingT rhs) { return lhs.value_ % rhs; }
    friend constexpr UnsignedWrapper operator%(UnderlyingT lhs, UnsignedWrapper rhs) { return lhs % rhs.value_; }

    // --- Symmetrical Bitwise with UnderlyingT ---

    friend constexpr UnsignedWrapper operator&(UnsignedWrapper lhs, UnderlyingT rhs) { return lhs.value_ & rhs; }
    friend constexpr UnsignedWrapper operator&(UnderlyingT lhs, UnsignedWrapper rhs) { return lhs & rhs.value_; }

    friend constexpr UnsignedWrapper operator|(UnsignedWrapper lhs, UnderlyingT rhs) { return lhs.value_ | rhs; }
    friend constexpr UnsignedWrapper operator|(UnderlyingT lhs, UnsignedWrapper rhs) { return lhs | rhs.value_; }

    friend constexpr UnsignedWrapper operator^(UnsignedWrapper lhs, UnderlyingT rhs) { return lhs.value_ ^ rhs; }
    friend constexpr UnsignedWrapper operator^(UnderlyingT lhs, UnsignedWrapper rhs) { return lhs ^ rhs.value_; }

    // --- Assignment with UnderlyingT ---
    UnsignedWrapper& operator+=(UnderlyingT rhs) { value_ += rhs; return *this; }
    UnsignedWrapper& operator-=(UnderlyingT rhs) { value_ -= rhs; return *this; }
    UnsignedWrapper& operator*=(UnderlyingT rhs) { value_ *= rhs; return *this; }
    UnsignedWrapper& operator/=(UnderlyingT rhs) { value_ /= rhs; return *this; }
    UnsignedWrapper& operator%=(UnderlyingT rhs) { value_ %= rhs; return *this; }
    UnsignedWrapper& operator&=(UnderlyingT rhs) { value_ &= rhs; return *this; }
    UnsignedWrapper& operator|=(UnderlyingT rhs) { value_ |= rhs; return *this; }
    UnsignedWrapper& operator^=(UnderlyingT rhs) { value_ ^= rhs; return *this; }

    // --- ostream types
    friend std::ostream& operator<<(std::ostream& os, UnsignedWrapper const& obj) { os << obj.value_; }
    // --- format types
    friend struct std::formatter<UnsignedWrapper<UnderlyingT>>;

private:
    UnderlyingT value_{0};
};
// --- hash types
namespace std {
    template <typename UnderlyingT>
    struct hash<UnsignedWrapper<UnderlyingT>> {
        size_t operator()(UnsignedWrapper<UnderlyingT> const& obj) {
            return std::hash<UnderlyingT>{}(obj.get());
        }
    };
} // namespace std
template <typename UnderlyingT>
struct std::formatter<UnsignedWrapper<UnderlyingT>> : std::formatter<T> {
    auto format(UnsignedWrapper<UnderlyingT> const& obj, std::format_context& ctx) const {
        return std::format<UnderlyingT>::format(obj.get(), ctx);
    }
};

template <typename UnderlyingT, bool is_bendian = true>
class Data : public UnsignedWrapper<UnderlyingT> {
    size_t unpack(uint8_t const* data) {
        if constexpr (is_bendian && size_of() == 8) { set(unpack64(data)); } 
        else if (is_bendian && size_of() == 4)      { set(unpack32(data)); }
        else if (is_bendian && size_of() == 2)      { set(unpack16(data)); }
        else if (is_bendian && size_of() == 1)      { set(unpack8(data));  }
        else if (!is_bendian && size_of() == 8)     { set(unpackle64(data)); }
        else if (!is_bendian && size_of() == 4)     { set(unpackle32(data)); }
        else if (!is_bendian && size_of() == 2)     { set(unpackle16(data)); }
        else if (!is_bendian && size_of() == 1)     { set(unpackle8(data)); }
        else {
            static_assert(false); // ill-formed before https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2593r1.html
        }
        return size_of();
    }
};

typedef Data<uint32_t> Data32;
typedef Data<uint64_t> Data64;
typedef Data<uint16_t> Data16;
typedef Data<uint8_t> Data8;

typedef Data<uint32_t, false> DataLE32;
typedef Data<uint64_t, false> DataLE64;
typedef Data<uint16_t, false> DataLE16;
typedef Data<uint8_t, false> DataLE8;
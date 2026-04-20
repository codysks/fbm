#pragma once

#include <cstdint>
#include "cppunpack.hpp"
#include <ostream>
#include <format>

template <typename UnderlyingT>
class UnsignedWrapper {
public:
    static constexpr size_t size_of(void) const noexcept { return sizeof(UnderlyingT); }
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
// --- format
template <typename UnderlyingT>
struct std::formatter<UnsignedWrapper<UnderlyingT>> : std::formatter<T> {
    auto format(UnsignedWrapper<UnderlyingT> const& obj, std::format_context& ctx) const {
        return std::format<UnderlyingT>::format(obj.get(), ctx);
    }
};

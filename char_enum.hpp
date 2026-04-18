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

struct SoupbinTcp_MessageType {
    enum class Enum : char {
        Unset = 0,
        // Sent by both
        Debug = '+',
        
        // Sent by Server
        LoginAccepted = 'A',
        LoginReject = 'J',
        SequencedData = 'S',
        ServerHeartbeat = 'H',
        EndOfSession = 'Z',
        
        // Sent by Client
        LoginRequest = 'L',
        UnsequencedData = 'U',
        ClientHeartbeat = 'R',
        LogoutRequest = 'O',
    };

    static constexpr Enum default_value(void) noexcept { return Enum::Unset; }
    static constexpr char underlying_value(Enum const e) { return static_cast<char>(e); }
    static constexpr std::string_view what(Enum const e) {
        using std::literals::string_view_literals::operator""sv;
        switch (e) {
            case Enum::Unset:           return "Unset"sv;
            case Enum::Debug:           return "Debug"sv;
            case Enum::LoginAccepted:   return "LoginAccepted"sv;
            case Enum::LoginReject:     return "LoginReject"sv;
            case Enum::SequencedData:   return "SequencedData"sv;
            case Enum::ServerHeartbeat: return "ServerHeartbeat"sv;
            case Enum::EndOfSession:    return "EndOfSession"sv;
            case Enum::LoginRequest:    return "LoginRequest"sv;
            case Enum::UnsequencedData: return "UnsequencedData"sv;
            case Enum::ClientHeartbeat: return "ClientHeartbeat"sv;
            case Enum::LogoutRequest:   return "LogoutRequest"sv;
        }
        assert(false && "Unimplemented");
        __builtin_unreachable();
    }
};

template <typename EnumDefinition>
class CharEnum : public EnumDefinition {
    using Enum = EnumDefinition::Enum;
public:
// Constructors
    CharEnum(void) noexcept {}
    template <typename T> CharEnum(T const a) noexcept;
    constexpr std::string_view what(void) const { return EnumDefinition::what(enumerate()); }
    bool operator==(Enum const e) { return enumerate() == e; }
    bool operator!=(Enum const e) { return !operator==(e); }
    constexpr Enum enumerate(void) const { return static_cast<Enum>(get()); }
    static constexpr Enum enumerate(char const c) { return static_cast<Enum>(c); }
    static consteval size_t length(void) noexcept { return 1; }
    operator char() { return value_; }
    template <typename T> CharEnum(T a) : value_(std::bit_cast<char>(a)) {
        static_assert(sizeof a == length());
        static_assert(std::is_integral_v<T>);
    };
    template <typename T>
    constexpr size_t unpack(T const a) {
        static_assert(sizeof(T) == length());
        static_assert(std::is_integral_v<T>);
        value_ = std::bit_cast<char>(a);
        return length();
    }
    template <typename T>
    constexpr size_t unpack(T const* a) {
        static_assert(sizeof(T) == length());
        static_assert(std::is_integral_v<T>);
        auto const v = *a;
        value_ = std::bit_cast<char>(a);
        return length();
    }
    constexpr char get(void) const { return value_; }
    operator std::string(void) { return std::string{{value_, }}; }
private:
    char value_ = EnumDefinition::underlying_value(EnumDefinition::default_value());
};

template <typename EnumDefinition>
struct std::formatter<CharEnum<EnumDefinition>, char> {
    bool simplified = false;
    constexpr auto parse(std::format_parse_context& ctx) {
        auto it = ctx.begin();
        if (*it == 's') {
            ++it;
            simplified = true;
        } else if (*it != '}') {
            return it;
        }
        if (*it == '}') {
            return it;
        }
        throw std::format_error("Invalid format");
    }
    auto format(CharEnum<EnumDefinition> const& data, std::format_context& ctx) const {
        if (simplified) {
            return std::format_to(ctx.out(), "{}", data.get());
        }
        return std::format_to(ctx.out(), "{}", data.what());
    }
};

typedef CharEnum<SoupbinTcp_MessageType> SoupbinTcpMessageType;

#include "char_enum.inl.hpp"
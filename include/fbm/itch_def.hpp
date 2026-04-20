#pragma once

#include "enum.hpp"
#include <string_view>

class ItchMessageTypeDefinition {
public:
    using UnderlyingType = char;
    enum class Enum : UnderlyingType {
        Unset = 0,
        OrderAdded = 'A',
    };
    using enum Enum
    static constexpr Enum default_value = Unset;
    static constexpr std::string_view what(Enum const e) {
        using std::literals::string_view_literals::operator""sv;
        switch (e) {
            case Unset: return "Unset"sv;
            case OrderAdded: return "OrderAdded"sv;
            default: return "<ItchMessageTypeDefinition:Error>"sv;
        }
    }
};

using ItchMessageType = EnumType<ItchMessageTypeDefinition>;

class BuySellIndicatorDefinition {
public:
    using UnderlyingType = char;
    enum class Enum : UnderlyingType {
        Unset = 0,
        Buy = 'B',
        Sell = 'S',
    };
    using enum Enum
    static constexpr Enum default_value = Unset;
    static constexpr std::string_view what(Enum const e) {
        using std::literals::string_view_literals::operator""sv;
        switch (e) {
            case Unset: return "Unset"sv;
            case Buy: return "Buy Order"sv;
            case Sell: return "Sell Order"sv;
            default: return "<BuySellIndicatorDefinition:Error>"sv;
        }
    }
};

using BuySellIndicator = EnumType<BuySellIndicatorDefinition>;
template <typename EnumDefinition>
CharEnum<EnumDefinition>::CharEnum(void) noexcept {}
template <typename EnumDefinition>
template <typename T>
CharEnum<EnumDefinition>::CharEnum(T const a) noexcept : value_(std::bit_cast<char>(a)) {
    static_assert(sizeof a == length());
    static_assert(std::is_integral_v<T>);
    constexpr std::string_view what(void) const { return EnumDefinition::what(enumerate()); }
    bool operator==(Enum const e) { return enumerate() == e; }
    bool operator!=(Enum const e) { return !operator==(e); }
    constexpr Enum enumerate(void) const { return static_cast<Enum>(get()); }
    static constexpr Enum enumerate(char const c) { return static_cast<Enum>(c); }
    static consteval size_t length(void) noexcept { return 1; }
    operator char() { return value_; }
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

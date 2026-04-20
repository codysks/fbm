#pragma once

template <typename EnumDefinition>
class EnumType : public EnumDefinition {
public:
    static constexpr size_t size_of(void) const noexcept { return 1; }
    size_t unpack(uint8_t const* data) {
        value_ = static_cast<EnumDefinition::Enum>(*data);
        return size_of();
    }
private:
    EnumDefinition::Enum value_ = EnumDefinition::default_value;
};
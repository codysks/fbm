
#include <cstdint>
#include "binary_data.hpp"
#include <cassert>
#include "itch_def.hpp"

namespace itch {
class OrderAddedMessage {
private:
    ItchMessageType message_type_;
    Data32 timestamp_nano_;
    Data64 order_number_;
    BuySellIndicator buy_sell_indicator_;
    Data32 quantity_;
    Data32 orderbook_id_;
    Data32 group_;
    Data32 price_;
public:
    constexpr size_t message_length(void) noexcept;
    size_t unpack(uint8_t const* data) noexcept;
};
constexpr size_t OrderAddedMessage::message_length(void) noexcept {
	constexpr size_t expected_length = 30;
// :execute "'<,'>normal0wwd0I        \<Esc>$R.size_of() +"
// 	  static_assert(false && "Unimplemented");
    constexpr size_t mlen =
        message_type_.size_of() +
        timestamp_nano_.size_of() +
        order_number_.size_of() +
        buy_sell_indicator_.size_of() +
        quantity_.size_of() +
        orderbook_id_.size_of() +
        group_.size_of() +
        price_.size_of() +
        0;
    static_assert(mlen == expected_length);
    return mlen;
}
size_t OrderAddedMessage::unpack(uint8_t const* data) noexcept
{
    auto pos = data;

	assert(pos - data == message_length());
    return pos - data;
}
} // namespace itch

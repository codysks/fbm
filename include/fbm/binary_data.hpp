
#pragma @once

#include "unsigned_wrapper.hpp"

template <typename UnderlyingT, bool is_bendian = true>
class Data : public UnsignedWrapper<UnderlyingT> {
public:
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
#ifndef HEADER_gubg_bit_sign_codec_hpp_ALREADY_INCLUDED
#define HEADER_gubg_bit_sign_codec_hpp_ALREADY_INCLUDED

#include <type_traits>

namespace gubg { namespace bit { namespace sign { 

    template <typename Int>
    std::make_unsigned_t<Int> encode(Int v)
    {
        return v < 0 ? ((-(v+1))*2+1) : (v*2);
    }

    template <typename UInt>
    std::make_signed_t<UInt> decode(UInt v)
    {
        using Int = std::make_signed_t<UInt>;
        return v&1u ? (-static_cast<Int>(v>>1)-1) : (static_cast<Int>(v>>1));
    }

} } } 

#endif

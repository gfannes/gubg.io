#ifndef HEADER_gubg_bit_oor_util_hpp_ALREADY_INCLUDED
#define HEADER_gubg_bit_oor_util_hpp_ALREADY_INCLUDED

#include <cstddef>

namespace gubg { namespace bit { namespace oor { namespace util { 

    template <typename T>
    constexpr T Max = T{}-1;

    template <typename T>
    constexpr std::size_t MaxBW = sizeof(T)*8u;

    template <typename T>
    std::size_t bitwidth(T v)
    {
        if (v == Max<T>)
            return MaxBW<T>+1;
        ++v;
        v >>= 1;
        std::size_t bw = 1u;
        for (; !!v; ++bw)
            v >>= 1;
        return bw;
    }

} } } } 

#endif

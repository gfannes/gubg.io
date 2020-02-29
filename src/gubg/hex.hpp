#ifndef HEADER_gubg_hex_hpp_ALREADY_INCLUDED
#define HEADER_gubg_hex_hpp_ALREADY_INCLUDED

#include <sstream>

namespace gubg { 

    namespace support { 
        template <typename T>
        char to_nibble(T v)
        {
            v &= 0x0f;

            if (v < 10)
                return '0'+v;

            return 'a'+(v-10);
        };

        template <std::size_t N, typename T>
        std::string hex(T v)
        {
            std::string res(2*N, '?');
            for (auto i = 0u; i < N; ++i)
            {
                auto ix = 2*N-2*i-1;
                res[ix] = to_nibble(v); v >>= 4;

                --ix;
                res[ix] = to_nibble(v); v >>= 4;
            }
            return res;
        }
    } 

    template <typename T>
    std::string hex(T v)
    {
        return support::hex<sizeof(T)>(v);
    }

} 

#endif

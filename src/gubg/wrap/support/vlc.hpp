#ifndef HEADER_gubg_wrap_support_vlc_hpp_ALREADY_INCLUDED
#define HEADER_gubg_wrap_support_vlc_hpp_ALREADY_INCLUDED

#include <cstddef>
#include <cstdint>
#include <vector>
#include <numeric>

namespace gubg { namespace wrap { namespace support { 

    template <typename Dst>
    void append_part16(Dst &dst, std::uint16_t part16)
    {
        const auto ix = dst.size();
        dst.resize(ix+2);
        dst[ix+1] = part16 & 0xff;
        part16 >>= 8;
        dst[ix+0] = part16 & 0xff;
    }

    template <typename Dst>
    void append_vlc(Dst &dst, std::size_t size, bool is_first = true)
    {
        //We recurse before we write our part: big endian at uint16 level
        if (size > 0x7fff)
            append_vlc(dst, size>>15, false);

        std::uint16_t part16 = is_first ? 0x8000 : 0x0000;

        part16 |= size & 0x7fff;

        append_part16(dst, part16);
    }

    template <typename Dst>
    void append_vlc(Dst &dst, const std::vector<bool> &flips, std::size_t offset = 0)
    {
        if (flips.size() > offset+15)
            append_vlc(dst, flips, offset+15);

        std::uint16_t part16 = (offset==0) ? 0x8000 : 0x0000;

        const auto end = std::min(flips.size(), offset+15);
        std::uint16_t mask = 1;
        for (auto ix = offset; ix < end; ++ix, mask<<=1)
            if (flips[ix])
                part16 |= mask;

        append_part16(dst, part16);
    }

} } } 

#endif

#ifndef HEADER_gubg_t2_Types_hpp_ALREADY_INCLUDED
#define HEADER_gubg_t2_Types_hpp_ALREADY_INCLUDED

#include <cstdint>

namespace gubg { namespace t2 { 

    using Byte = std::uint8_t;
    using Data = std::uint32_t;

    //Meta-data mask: top-two bits
    static constexpr Byte mask_md        = 0b11000000; 
    static constexpr Byte mask_data      = 0b00111111; 

    //Meta-data values
    static constexpr Byte md_special     = 0b00000000; 
    static constexpr Byte md_open_tag    = 0b01000000; 
    static constexpr Byte md_open_attr   = 0b10000000; 
    static constexpr Byte md_data        = 0b11000000; 

    //Special values
    static constexpr Byte md_som         = md_special | 0x00;
    static constexpr Byte md_eom         = md_special | 0x01;
    static constexpr Byte md_close_block = md_special | 0x02;

} } 

#endif

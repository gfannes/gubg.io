#ifndef HEADER_gubg_wav_Format_hpp_ALREADY_INCLUDED
#define HEADER_gubg_wav_Format_hpp_ALREADY_INCLUDED

#include <cstdint>

namespace gubg { namespace wav { 

    class Format
    {
    public:
        //Do not change these types as they are used during parsing
        std::uint16_t format;
        std::uint16_t channel_count;
        std::uint32_t sample_rate;
        std::uint32_t byte_rate;
        std::uint16_t block_align;
        std::uint16_t bits_per_sample;
    private:
    };

} } 

#endif

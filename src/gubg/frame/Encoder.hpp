#ifndef HEADER_gubg_frame_Encoder_hpp_ALREADY_INCLUDED
#define HEADER_gubg_frame_Encoder_hpp_ALREADY_INCLUDED

#include <gubg/mss.hpp>
#include <cstdint>
#include <vector>
#include <cassert>

namespace gubg { namespace frame { 

    class Encoder
    {
    public:
        std::vector<std::uint8_t> header;

        Encoder(std::uint8_t soh): soh_(soh) { }

        //Payload will be flipped in-place
        bool encode(std::uint64_t type, std::vector<std::uint8_t> &payload)
        {
            MSS_BEGIN(bool, "");

            header.resize(0);

            header.push_back(soh_);
            add_to_header_(type);
            add_to_header_(payload.size());

            unsigned int nr = 0;
            std::uint8_t flips = 0;
            auto add_nr_if_above = [&](unsigned int max){
                if (nr > max)
                {
                    nr ^= soh_;
                    assert(nr != soh_);
                    header.push_back(nr);
                    nr = 0;
                }
            };
            auto process_flips = [&](){
                if (flips == 0)
                {
                    //No bytes were flipped in this 7-byte run: add this info to "nr"
                    ++nr;
                    //Add nr if it outgrows its 7-bit space
                    add_nr_if_above(126);
                }
                else
                {
                    //At least one byte was flipped in this 7-byte run
                    add_nr_if_above(0);
                    flips |= 0x80;
                    flips ^= soh_;
                    assert(flips != soh_);
                    header.push_back(flips);
                    flips = 0;
                }
            };
            unsigned int offset = 0;
            for (auto &byte: payload)
            {
                if (byte == soh_)
                {
                    L("Flip the payload byte (" << std::hex << (int)byte << std::dec << ")");
                    byte ^= 0x01;
                    flips |= (1<<offset);
                    L(" => " << std::hex << (int)byte << " " << flips << std::dec);
                }

                ++offset;
                if (offset == 7)
                {
                    process_flips();
                    offset = 0;
                }
            }

            if (offset > 0)
                process_flips();
            add_nr_if_above(0);

            MSS_END();
        }

    private:
        //Writes an unsigned VLC number using 6 bits of data in big endian:
        //bit7: set to zero to ensure the byte can never be zero (==soh after xor)
        //bit6: set to 1 if this is the last part of the VLC
        //bit5-bit0: data
        void add_to_header_(std::uint64_t v, bool msbits = true)
        {
            if (v >= 64)
                //Recursion to get big endian
                add_to_header_(v >> 6, false);

            std::uint8_t byte = 0x80;
            byte |= (msbits ? 0x40 : 0x00);
            byte |= (v & 0x3f);
            byte ^= soh_;
            assert(byte != soh_);
            header.push_back(byte);
        }

        const std::uint8_t soh_;
    };

} } 

#endif

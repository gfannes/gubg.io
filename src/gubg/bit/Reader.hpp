#ifndef HEADER_gubg_bit_Reader_hpp_ALREADY_INCLUDED
#define HEADER_gubg_bit_Reader_hpp_ALREADY_INCLUDED

#include <gubg/mss.hpp>
#include <cstdint>
#include <vector>
#include <cassert>

namespace gubg { namespace bit { 

    class Reader
    {
    public:
        Reader() {}
        template <typename Byte>
        Reader(const Byte *ptr, std::size_t size)
        {
            setup(ptr, size);
        }

        template <typename Byte>
        bool setup(const Byte *ptr, std::size_t size)
        {
            static_assert(sizeof(Byte) == 1, "bit::Reader only works on bytes");

            MSS_BEGIN(bool);

            bits_.resize(size*8u);
            ix_ = 0u;
            for (auto ix = 0u; ix < size; ++ix)
            {
                const auto byte = (std::uint8_t)ptr[ix];
                append_nibble_(byte & 0x0fu);
                append_nibble_(byte >> 4);
            }
            ix_ = 0u;

            MSS_END();
        }

        std::size_t bit_size() const {return bits_.size() - ix_;}

        bool zero()
        {
            if (ix_ < bits_.size())
                return (bits_[ix_++] == 0);
            return true;
        }
        bool one()
        {
            if (ix_ < bits_.size())
                return (bits_[ix_++] == 1);
            return false;
        }

        void bit(bool &b)
        {
            assert(ix_ < bits_.size());
            b = (bits_[ix_++] == 1);
        }

        template <typename UInt>
        void uint(UInt &v, std::size_t bit_count = sizeof(UInt))
        {
            const auto ix_end = ix_+bit_count;
            assert(ix_end <= bits_.size());
            v = 0u;
            for (auto ix = ix_end; ix > ix_; )
            {
                --ix;
                v <<= 1;
                v += bits_[ix];
            }
            ix_ += bit_count;
        }

    private:
        void append_nibble_(std::uint8_t byte)
        {
            auto append = [&](std::uint8_t b0, std::uint8_t b1, std::uint8_t b2, std::uint8_t b3){
                bits_[ix_+0] = b0;
                bits_[ix_+1] = b1;
                bits_[ix_+2] = b2;
                bits_[ix_+3] = b3;
            };
            switch (byte)
            {
                case 0:  append(0,0,0,0); break;
                case 1:  append(1,0,0,0); break;
                case 2:  append(0,1,0,0); break;
                case 3:  append(1,1,0,0); break;
                case 4:  append(0,0,1,0); break;
                case 5:  append(1,0,1,0); break;
                case 6:  append(0,1,1,0); break;
                case 7:  append(1,1,1,0); break;
                case 8:  append(0,0,0,1); break;
                case 9:  append(1,0,0,1); break;
                case 10: append(0,1,0,1); break;
                case 11: append(1,1,0,1); break;
                case 12: append(0,0,1,1); break;
                case 13: append(1,0,1,1); break;
                case 14: append(0,1,1,1); break;
                case 15: append(1,1,1,1); break;
                default: assert(false); break;
            }
            ix_ += 4;
        }

        std::vector<std::uint8_t> bits_;
        std::size_t ix_ = 0;

    };

} } 

#endif

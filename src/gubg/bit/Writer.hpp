#ifndef HEADER_gubg_bit_Writer_hpp_ALREADY_INCLUDED
#define HEADER_gubg_bit_Writer_hpp_ALREADY_INCLUDED

#include <cstdint>
#include <vector>
#include <array>
#include <cassert>

namespace gubg { namespace bit { 

    class Writer
    {
    public:
        Writer()
        {
            setup_masks_();
        }

        void clear()
        {
            bits_.clear();
        }

        std::size_t bit_size() const  {return bits_.size();}
        std::size_t byte_size() const {return (bit_size()+7u)/8u;}

        void one()       { bits_.emplace_back(masks_[mask_ix_()]); }
        void zero()      { bits_.emplace_back(0u); }
        void bit(bool b) { bits_.emplace_back(b ? masks_[mask_ix_()] : 0u); }

        template <typename UInt>
        void uint(const UInt v, std::size_t bit_count = sizeof(UInt))
        {
            UInt mask = 1u;
            for (auto ix = 0u; ix < bit_count; ++ix, mask <<= 1)
                bit(v & mask);
        }

        template <typename Bytes>
        void to_bytes(Bytes &bytes)
        {
            const auto full_bytes = bit_size()/8;
            const auto last_bits = bit_size()%8;

            bytes.resize(full_bytes + !!last_bits);

            auto bit = bits_.begin();
            for (auto ix = 0u; ix < full_bytes; ++ix, bit += 8)
                bytes[ix] = bit[0]|bit[1]|bit[2]|bit[3]|bit[4]|bit[5]|bit[6]|bit[7];

            if (last_bits)
            {
                auto &byte = bytes.back();
                byte = 0u;
                for (auto ix = 0u; ix < last_bits; ++ix, ++bit)
                    byte |= *bit;
            }

            assert(bit == bits_.end());
        }

        template <typename Bits>
        void to_bits(Bits &bits)
        {
            const auto size = bits_.size();
            bits.resize(size);
            for (auto ix = 0u; ix < size; ++ix)
                bits[ix] = !!bits_[ix];
        }

    private:
        void setup_masks_()
        {
            for (auto ix = 0u; ix < masks_.size(); ++ix)
                //Little-endian bits
                masks_[ix] = (1u << ix);
        }
        std::size_t mask_ix_() const
        {
            return bit_size() & 0x07;
        }

        std::array<std::uint8_t, 8> masks_;

        std::vector<std::uint8_t> bits_;
    };

} } 

#endif

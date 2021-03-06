#ifndef HEADER_gubg_bit_gr_Sequence_hpp_ALREADY_INCLUDED
#define HEADER_gubg_bit_gr_Sequence_hpp_ALREADY_INCLUDED

#include <gubg/bit/gr/Codec.hpp>
#include <gubg/bit/Writer.hpp>
#include <gubg/bit/Reader.hpp>
#include <gubg/ilog.hpp>

namespace gubg { namespace bit { namespace gr { 

    template <Type MyType>
    class Sequence
    {
    public:
        Sequence()
        {
            reset(0u);
        }

        void reset(unsigned int remainder_bw)
        {
            enc_md_.remainder_bw = remainder_bw;
            dec_md_.remainder_bw = remainder_bw;
            count_ = 0u;
            std::cout << std::endl;
        }

        std::size_t count() const {return count_;}

        template <typename T>
        void encode(Writer &writer, T v)
        {
            constexpr bool is_signed = std::is_signed_v<T>;
            if constexpr (!is_signed)
                encode_(writer, v);
            if constexpr (is_signed)
                encode_(writer, sign::encode(v));
        }
        template <typename T>
        void encode(Writer &writer, const T *ptr, std::size_t size)
        {
            for (auto ix = 0u; ix < size; ++ix)
                encode(writer, ptr[ix]);
        }

        template <typename T>
        void decode(T &v, Reader &reader)
        {
            constexpr bool is_signed = std::is_signed_v<T>;
            if constexpr (!is_signed)
                decode_(v, reader);
            if constexpr (is_signed)
            {
                unsigned int u;
                decode_(u, reader);
                v = sign::decode(u);
            }
        }
        template <typename T>
        void decode(T *ptr, std::size_t size, Reader &reader)
        {
            for (auto ix = 0u; ix < size; ++ix)
                decode(ptr[ix], reader);
        }

    private:
        void update_remainder_(unsigned int &remainder, unsigned int u)
        {
            const auto optimal_remainder = ilog2(u);
            if constexpr (MyType == Type::Normal)
            {
                if (optimal_remainder >= remainder)
                    remainder = optimal_remainder;
                else if (remainder > 2)
                    --remainder;
            }
            if constexpr (MyType == Type::Exponential)
            {
                if (optimal_remainder >= remainder)
                    remainder = optimal_remainder;
                else
                    --remainder;
                    /* remainder = optimal_remainder; */
            }
            std::cout << remainder << ", ";
        }
        void encode_(Writer &writer, unsigned int u)
        {
            codec_.encode(writer, enc_md_, u);
            update_remainder_(enc_md_.remainder_bw, u);
            ++count_;
        }
        void decode_(unsigned int &u, Reader &reader)
        {
            codec_.decode(u, dec_md_, reader);
            update_remainder_(dec_md_.remainder_bw, u);
            --count_;
        }

        Metadata enc_md_;
        Metadata dec_md_;
        Codec<unsigned int, MyType> codec_;
        unsigned int count_ = 0u;
    };

} } } 

#endif

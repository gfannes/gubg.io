#ifndef HEADER_gubg_bit_gr_Sequence_hpp_ALREADY_INCLUDED
#define HEADER_gubg_bit_gr_Sequence_hpp_ALREADY_INCLUDED

#include <gubg/bit/gr/Codec.hpp>
#include <gubg/bit/Writer.hpp>
#include <gubg/bit/Reader.hpp>
#include <gubg/ilog.hpp>

namespace gubg { namespace bit { namespace gr { 

    template <typename T, Type MyType>
    class Sequence
    {
    private:
        static constexpr bool is_signed = std::is_signed_v<T>;

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
        }

        std::size_t count() const {return count_;}

        void encode(Writer &writer, T v)
        {
            if constexpr (!is_signed)
                encode_(writer, v);
            if constexpr (is_signed)
                encode_(writer, sign::encode(v));
        }
        void encode(Writer &writer, const T *ptr, std::size_t size)
        {
            for (auto ix = 0u; ix < size; ++ix)
                encode(writer, ptr[ix]);
        }

        void decode(T &v, Reader &reader)
        {
            if constexpr (!is_signed)
                decode_(v, reader);
            if constexpr (is_signed)
            {
                UInt u;
                decode_(u, reader);
                v = sign::decode(u);
            }
        }
        void decode(T *ptr, std::size_t size, Reader &reader)
        {
            for (auto ix = 0u; ix < size; ++ix)
                decode(ptr[ix], reader);
        }

    private:
        using UInt = std::make_unsigned_t<T>;

        void encode_(Writer &writer, UInt u)
        {
            codec_.encode(writer, enc_md_, u);
            enc_md_.remainder_bw = ilog2(u);
            ++count_;
        }
        void decode_(UInt &u, Reader &reader)
        {
            codec_.decode(u, dec_md_, reader);
            dec_md_.remainder_bw = ilog2(u);
            --count_;
        }

        Metadata enc_md_;
        Metadata dec_md_;
        Codec<UInt, MyType> codec_;
        unsigned int count_ = 0u;
    };

} } } 

#endif

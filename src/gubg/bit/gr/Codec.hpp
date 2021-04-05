#ifndef HEADER_gubg_bit_gr_Codec_hpp_ALREADY_INCLUDED
#define HEADER_gubg_bit_gr_Codec_hpp_ALREADY_INCLUDED

#include <gubg/bit/gr/Type.hpp>
#include <gubg/bit/Writer.hpp>
#include <gubg/bit/Reader.hpp>
#include <gubg/bit/sign/codec.hpp>
#include <gubg/ilog.hpp>
#include <type_traits>
#include <cassert>

namespace gubg { namespace bit { namespace gr { 

    struct Metadata
    {
        unsigned int remainder_bw = 0;

        Metadata() {}
        Metadata(unsigned int remainder_bw): remainder_bw(remainder_bw) {}
    };

    template <typename T, Type MyType>
    class Codec
    {
    private:
        static constexpr bool is_signed = std::is_signed_v<T>;

    public:
        void encode(Writer &writer, const Metadata &md, T v)
        {
            if constexpr (!is_signed)
                encode_(writer, md, v);
            if constexpr (is_signed)
                encode_(writer, md, sign::encode(v));
        }

        void decode(T &v, const Metadata &md, Reader &reader)
        {
            if constexpr (!is_signed)
                decode_(v, md, reader);
            if constexpr (is_signed)
            {
                UInt u;
                decode_(u, md, reader);
                v = sign::decode(u);
            }
        }

    private:
        using UInt = std::make_unsigned_t<T>;

        void encode_(Writer &writer, const Metadata &md, UInt u)
        {
            writer.uint(u, md.remainder_bw);
            u >>= md.remainder_bw;
            if constexpr (MyType == Type::Normal)
            {
                writer.ones(u);
                writer.zero();
            }
            if constexpr (MyType == Type::Exponential)
            {
                const unsigned int quotient_bw = ilog2(u);
                writer.ones(quotient_bw);
                writer.zero();
                writer.uint(u, quotient_bw);
            }
        }

        void decode_(UInt &u, const Metadata &md, Reader &reader)
        {
            T remainder;
            reader.uint(remainder, md.remainder_bw);
            if constexpr (MyType == Type::Normal)
            {
                for (u = 0u; reader.one(); ++u) { }
            }
            if constexpr (MyType == Type::Exponential)
            {
                unsigned int quotient_bw = 0u;
                for (quotient_bw = 0u; reader.one(); ++quotient_bw) { }
                reader.uint(u, quotient_bw);
            }
            u <<= md.remainder_bw;
            u |= remainder;
        }
    };

} } } 

#endif

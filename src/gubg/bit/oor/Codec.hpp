#ifndef HEADER_gubg_bit_oor_Codec_hpp_ALREADY_INCLUDED
#define HEADER_gubg_bit_oor_Codec_hpp_ALREADY_INCLUDED

#include <gubg/bit/Writer.hpp>
#include <gubg/bit/Reader.hpp>
#include <gubg/ilog.hpp>
#include <cstdint>
#include <array>
#include <cassert>

namespace gubg { namespace bit { namespace oor { 

    template <typename T>
    class Codec
    {
    public:
        //This metadata is needed to decode a bitstream again into a std::vector<T>,
        //together with its size
        struct Metadata
        {
            unsigned int max_bw = 0;
            unsigned int min_bw = 0;
        };

        void encode(Metadata &md, Writer &writer, const T *data, std::size_t size)
        {
            compute_statistics_(data, size);

            find_optimal_metadata_(md);

            assert(md.min_bw < MaxBW);
            const T oor_marker = (1u<<md.min_bw)-1;

            for (auto ix = 0u; ix < size; ++ix)
            {
                const auto x = data[ix];

                if (x < oor_marker)
                {
                    writer.uint(x, md.min_bw);
                }
                else
                {
                    writer.uint(oor_marker, md.min_bw);
                    writer.uint(x, md.max_bw);
                }
            }
        }

        void decode(T *data, std::size_t size, const Metadata &md, Reader &reader)
        {
            assert(md.min_bw < MaxBW);
            const T oor_marker = (1u<<md.min_bw)-1;

            for (auto ix = 0u; ix < size; ++ix)
            {
                auto &x = data[ix];
                reader.uint(x, md.min_bw);
                if (x == oor_marker)
                    reader.uint(x, md.max_bw);
            }
        }

    private:
        static constexpr std::size_t MaxBW = sizeof(T)*8u;

        void find_optimal_metadata_(Metadata &best_md) const
        {
            best_md.max_bw = max_bw_;

            //For bw==0, all data is out-of-range
            unsigned int oor_count = size_;
            unsigned int best_bit_count = size_*max_bw_;
            best_md.min_bw = 0u;

            for (auto bw = 0u; bw < MaxBW; ++bw)
            {
                oor_count -= bw__count_[bw];

                const auto bit_count = size_*bw + max_bw_*oor_count;
                if (bit_count < best_bit_count)
                {
                    best_md.min_bw = bw;
                    best_bit_count = bit_count;
                }
            }
        }

        void compute_statistics_(const T *data, std::size_t size)
        {
            max_bw_ = 0u;
            size_ = size;

            std::fill(bw__count_.begin(), bw__count_.end(), T{});
            for (auto ix = 0u; ix < size; ++ix)
            {
                const auto x = data[ix];

                //Number of bits required to write an out-of-range value
                const auto ilog2 = gubg::ilog2(x);
                max_bw_ = std::max<unsigned int>(max_bw_, ilog2);

                //Adjust ilog2 for those cases where x is of the form 0x00001111
                const auto bw = ilog2 + (((1u<<ilog2)-1u)==x);

                ++bw__count_[bw];
            }
        }

        unsigned int max_bw_;
        unsigned int size_;
        std::array<std::size_t, MaxBW+1> bw__count_;
    };

} } } 

#endif

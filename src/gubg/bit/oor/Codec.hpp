#ifndef HEADER_gubg_bit_oor_Codec_hpp_ALREADY_INCLUDED
#define HEADER_gubg_bit_oor_Codec_hpp_ALREADY_INCLUDED

#include <cstdint>
#include <array>

#include <iostream>

namespace gubg { namespace bit { namespace oor { 

    template <typename T>
    class Codec
    {
    public:
        void encode(const T *data, std::size_t size)
        {
            compute_statistics_(data, size);

            Params params;
            /* find_optimal_param_(params); */
            std::cout << "optimal params: " << params.bw << " " << params.bitcount << std::endl;
        }

    private:
        static constexpr std::size_t MaxBW = sizeof(T)*8u;

        struct Params
        {
            std::size_t bw = 0;
            std::size_t bitcount = 0;
        };

        void find_optimal_param_(Params &optimal_params) const
        {
            for (std::size_t params = MaxBW; params >= 1; --params)
            {
            }
        }

        std::size_t bitcount_(std::size_t bw) const
        {
            return 0;
        }

        void compute_statistics_(const T *data, std::size_t size)
        {
            std::fill(bw__count_.begin(), bw__count_.end(), T{});
            for (auto ix = 0u; ix < size; ++ix)
            {
                const auto bw = bitwidth_(data[ix]);
                ++bw__count_[bw];
            }

#if 0
            for (auto bw = 0u; bw < bw__count_.size(); ++bw)
                std::cout << bw << " " << bw__count_[bw] << std::endl;
#endif
        }
        std::size_t bitwidth_(T v)
        {
            if (v == ~T{})
                return MaxBW;
            ++v;
            v >>= 1;
            std::size_t bw = 1u;
            for (; !!v; ++bw)
                v >>= 1;
            return bw;
        }

        std::array<std::size_t, MaxBW+1> bw__count_;
    };

} } } 

#endif

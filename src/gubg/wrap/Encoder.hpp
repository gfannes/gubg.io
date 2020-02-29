#ifndef HEADER_gubg_wrap_Encoder_hpp_ALREADY_INCLUDED
#define HEADER_gubg_wrap_Encoder_hpp_ALREADY_INCLUDED

#include <gubg/wrap/types.hpp>
#include <gubg/wrap/support/vlc.hpp>
#include <gubg/Range.hpp>
#include <gubg/mss.hpp>
#include <vector>
#include <algorithm>
#include <cstdint>

namespace gubg { namespace wrap { 

    class Encoder
    {
    public:
        void operator()(PDU &pdu, const SDU &sdu)
        {
            prepare_header_(sdu);

            prepare_data_(sdu);

            pdu.resize(header_.size()+data_.size());

            auto dst = pdu.begin();
            std::copy(RANGE(header_), dst);
            dst += header_.size();
            std::copy(RANGE(data_), dst);
        }

    private:
        void prepare_header_(const SDU &sdu)
        {
            header_.resize(0);

            header_.push_back(0xdc);
            header_.push_back(0xd1);

            support::append_vlc(header_, sdu.size());

            compute_flips_(sdu);
            support::append_vlc(header_, flips_);
        }
        void prepare_data_(const SDU &sdu)
        {
            data_.resize(sdu.size());
            //TODO: take the flips into account
            std::copy(RANGE(sdu), data_.begin());
        }
        void compute_flips_(const SDU &sdu)
        {
            flips_.resize(0);
            for (auto i = 0u; i < sdu.size()/2; ++i)
            {
                const auto ix = 2*i;
                const std::uint16_t msb = static_cast<std::uint16_t>(sdu[ix]  ) << 8;
                const std::uint16_t lsb = static_cast<std::uint16_t>(sdu[ix+1]);
                const std::uint16_t part16 = msb|lsb;
                switch (part16)
                {
                    case 0xdcd0: flips_.push_back(false); break;
                    case 0xdcd1: flips_.push_back(true); break;
                    default: break;
                }
            }
        }

        std::vector<std::uint8_t> header_;
        std::vector<std::uint8_t> data_;
        std::vector<bool> flips_;
    };

} } 

#endif

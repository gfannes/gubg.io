#ifndef HEADER_gubg_wrap_Encoder_hpp_ALREADY_INCLUDED
#define HEADER_gubg_wrap_Encoder_hpp_ALREADY_INCLUDED

#include <gubg/wrap/types.hpp>
#include <gubg/wrap/my/Detector.hpp>
#include <gubg/Range.hpp>
#include <gubg/mss.hpp>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <set>
#include <optional>
#include <cassert>

namespace gubg { namespace wrap { 

    //PDU = [SOM | vlc6(FLIPS)] | flipped(vlc7(SIZE) | SDU)

    class Encoder
    {
    public:
        Encoder(const std::string &som = ""):
        som_(som),
        som_start_detector_(som.substr(0, (som.empty()?0:som.size()-1))),
        som_last_orig_(som.empty()?'\0':som.back())
        { }

        bool operator()(PDU &pdu, const SDU &sdu)
        {
            MSS_BEGIN(bool);

            MSS(check_som_());

            compute_raw_size_sdu_(sdu);

            compute_flip_bits_(sdu);
            compute_flips_();

            pdu.resize(som_.size()+flips_.size()+size_sdu_.size());

            auto dst = pdu.begin();
            auto copy = [&](const auto &src)
            {
                std::copy(RANGE(src), dst);
                dst += src.size();
            };
            copy(som_);
            copy(flips_);
            copy(size_sdu_);

            MSS_END();
        }

    private:
        bool check_som_()
        {
            MSS_BEGIN(bool);

            if (!som_is_ok_)
            {
                //Check that all som bytes and the flipped version of the last byte are different
                std::set<char> byte__presence;
                for (auto ch: som_)
                    byte__presence.insert(ch);
                byte__presence.insert(som_last_flipped_);

                som_is_ok_ = (byte__presence.size() == som_.size()+1);
            }
            assert(!!som_is_ok_);

            MSS(*som_is_ok_, std::cout << "Error: SOM characters should be all different, as should be the inverse of the last." << std::endl);

            MSS_END();
        }

        void append_size_as_vlc7_(std::size_t size, bool is_first = true)
        {
            //We recurse before we write our part: big endian at uint16 level
            if (size > 0x7f)
                append_size_as_vlc7_(size>>7, false);

            size_sdu_.push_back(is_first ? 0x80 : 0x00);
            size_sdu_.back() |= (size & 0x7f);
        }
        void compute_raw_size_sdu_(const SDU &sdu)
        {
            size_sdu_.resize(0);

            append_size_as_vlc7_(sdu.size());

            const auto sdu_begin_ix = size_sdu_.size();
            size_sdu_.resize(sdu_begin_ix+sdu.size());

            std::copy(RANGE(sdu), &size_sdu_[sdu_begin_ix]);
        }

        void compute_flip_bits_(const SDU &sdu)
        {
            flip_bits_.resize(0);

            if (som_.empty())
                //User wants no SOM, we will also not include flips_ in the PDU
                return;

            for (auto &ch: size_sdu_)
            {
                if (som_start_detector_.found_sequence())
                {
                    //We found the start of the SOM already, ch is the last char of the SOM

                    if (false) {}
                    else if (ch == som_last_orig_)
                    {
                        //Flip the last byte and indicate we had to flip it
                        flip_bits_.push_back(true);
                        ch = som_last_flipped_;
                    }
                    else if (ch == som_last_flipped_)
                    {
                        //Indicate that we have a false-positive match and that
                        //the decoder should not flip this last byte
                        flip_bits_.push_back(false);
                    }
                }
                som_start_detector_(ch);
            }
        }
        void compute_flips_()
        {
            flips_.resize(0);

            if (som_.empty())
                //User wants no SOM, we will also not include flips_ in the PDU
                return;

            //Convert the flip_bits_ into the vlc6-coded flips_
            {
                //We initialize the msbit with the msbit of the inverse of som_[0]
                //to ensure there cannot be a match with som_ in flips_
                const char init = (~som_[0])&0b10000000;

                for (auto ix = 0u; ix < flip_bits_.size(); ++ix)
                {
                    const auto mod = ix%6;
                    if (mod == 0)
                        //Add new byte to flips_
                        flips_.push_back(init);
                    if (flip_bits_[ix])
                        //Indicate that we flipped here
                        flips_.back() |= (1<<mod);
                }
                if (flips_.empty())
                    flips_.push_back(init);
                //Indicate this was the last byte
                flips_.back() |= 0b01000000;
            }
        }

        const std::string som_;
        std::optional<bool> som_is_ok_;
        my::Detector som_start_detector_;
        const char som_last_orig_ = '\0';
        const char som_last_flipped_ = ~som_last_orig_;

        std::string flips_;
        std::string size_sdu_;

        std::vector<bool> flip_bits_;
    };

} } 

#endif

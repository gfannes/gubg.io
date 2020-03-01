#ifndef HEADER_gubg_wrap_Encoder_hpp_ALREADY_INCLUDED
#define HEADER_gubg_wrap_Encoder_hpp_ALREADY_INCLUDED

#include <gubg/wrap/types.hpp>
#include <gubg/Range.hpp>
#include <gubg/mss.hpp>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <set>

namespace gubg { namespace wrap { 

    //PDU = [SOM | vlc6(FLIPS)] | flipped(vlc7(SIZE) | SDU)

    class Encoder
    {
    public:
        Encoder(const std::string &som = ""): som_(som)
        {
        }

        bool operator()(PDU &pdu, const SDU &sdu)
        {
            MSS_BEGIN(bool);

            MSS(prepare_som_());

            compute_raw_size_sdu_(sdu);

            compute_flips_(sdu);

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
        bool prepare_som_()
        {
            MSS_BEGIN(bool);

            if (som_is_prepared_)
                return true;

            const auto size = som_.size();
            if (size > 0)
            {
                //SOM without last byte
                som_start_ = som_;
                som_start_.resize(size-1);

                //Original and flipped version of the last byte
                som_last_orig_ = som_[size-1];
                som_last_flipped_ = ~som_last_orig_;

                //Check that all som bytes and the flipped version of the last byte are different
                {
                    std::set<char> byte__presence;
                    for (auto ch: som_)
                        byte__presence.insert(ch);
                    byte__presence.insert(som_last_flipped_);

                    MSS(byte__presence.size() == som_.size()+1, std::cout << "Error: SOM characters should be all different, as should be the inverse of the last." << std::endl);
                }
            }
            som_is_prepared_ = true;

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

        void compute_flips_(const SDU &sdu)
        {
            flip_bits_.resize(0);
            flips_.resize(0);

            if (som_.empty())
                //User wants no SOM, we will also not include flips_ in the PDU
                return;

            //Compute flip_bits_, only when the som actually fits in the size_sdu_
            if (som_.size() <= size_sdu_.size())
            {
                const std::size_t end_ix = size_sdu_.size()-som_.size()+1;
                const auto som_start_size = som_start_.size();
                auto som_start_match_at = [&](std::size_t offset)
                {
                    for (auto ix = 0u; ix < som_start_size; ++ix)
                        if (som_start_[ix] != size_sdu_[offset+ix])
                            return false;
                    return true;
                };
                for (auto ix = 0u; ix < end_ix; ++ix)
                    if (som_start_match_at(ix))
                    {
                        auto &last_char = size_sdu_[ix+som_start_size];
                        if (false) {}
                        else if (last_char == som_last_orig_)
                        {
                            //Flip the last byte and indicate we had to flip it
                            flip_bits_.push_back(true);
                            last_char = som_last_flipped_;
                        }
                        else if (last_char == som_last_flipped_)
                        {
                            //Indicate that we have a false-positive match and that
                            //the decoder should not flip this last byte
                            flip_bits_.push_back(false);
                        }
                    }
            }

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
        bool som_is_prepared_ = false;
        std::string som_start_;
        char som_last_orig_;
        char som_last_flipped_;

        std::string flips_;
        std::string size_sdu_;

        std::vector<bool> flip_bits_;
    };

} } 

#endif

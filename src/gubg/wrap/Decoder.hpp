#ifndef HEADER_gubg_wrap_Decoder_hpp_ALREADY_INCLUDED
#define HEADER_gubg_wrap_Decoder_hpp_ALREADY_INCLUDED

#include <gubg/wrap/my/Detector.hpp>
#include <gubg/wrap/my/som.hpp>
#include <gubg/wrap/types.hpp>
#include <gubg/mss.hpp>
#include <gubg/hex.hpp>
#include <vector>
#include <sstream>

namespace gubg { namespace wrap { 

#define unroll_states(x) x(Idle) x(FlipBits) x(Size) x(RawSDU) x(SDU) x(Error)
    enum class State
    {
#define x(name) name,
        unroll_states(x)
#undef x
    };
    inline const char * hr(State state)
    {
        switch (state)
        {
#define x(name) case State::name: return #name;
            unroll_states(x)
#undef x
        }
        return "<unknown state>";
    }
#undef unroll_states

    class Decoder
    {
    public:
        std::vector<SDU> received_sdus;
        std::vector<std::string> errors;

        Decoder(const std::string som): som_(som), detector_(som), flipped_detector_(my::flipped(som)) {}

        void process(const char *begin, const char *end)
        {
            for (auto it = begin; it != end; ++it)
                process(*it);
        }
        void process(const std::string &str)
        {
            for (auto ch: str)
                process(ch);
        }
        void process(char ch)
        {
            if (detector_.found_sequence())
            {
                //We found the SOM sequence and are ready to process the first byte after the SOM
                change_state_(State::FlipBits);
            }
            detector_(ch);
            switch (state_)
            {
                case State::Idle:
                    //Nothing todo, dumping data
                    break;
                case State::FlipBits:
                    {
                        const auto offset = flip_bits_.size();
                        flip_bits_.resize(offset+6);
                        for (auto i = 0u; i < 6; ++i)
                        {
                            const auto ix = i+offset;
                            flip_bits_[ix] = ch&(1<<i);
                        }
                        if (ch & 0b01000000)
                            //This was the last flip bits byte
                            change_state_(State::Size);
                    }
                    break;
                case State::Size:
                    {
                        size_ <<= 7;
                        size_ |= ch&0x7f;
                        if (ch & 0b10000000)
                            //This was the last size byte
                            change_state_(State::RawSDU);
                    }
                    break;
                case State::RawSDU:
                    sdu_[sdu_ix_] = ch;
                    ++sdu_ix_;
                    if (sdu_ix_ == sdu_.size())
                        change_state_(unflip_sdu_() ? State::SDU : State::Error);
                    break;
            }
        }

    private:
        bool unflip_sdu_()
        {
            MSS_BEGIN(bool);

            if (som_.empty())
                return true;

            flipped_detector_.reset();
            auto flip_it = flip_bits_.begin();
            for (auto &ch: sdu_)
            {
                flipped_detector_(ch);
                if (flipped_detector_.found_sequence())
                {
                    MSS(flip_it != flip_bits_.end());
                    if (*flip_it)
                        ch = som_.back();
                    ++flip_it;
                }
            }

            MSS_END();
        }
        void change_state_(State new_state)
        {
            S(nullptr);
            if (new_state == state_)
                return;

            auto check_new_state = [&](State wanted_state)
            {
                if (new_state == wanted_state)
                    return;
                oss_ << "Unexpected state transition from " << hr(state_) << " to " << hr(new_state) << " while " << hr(wanted_state) << " was expected";
                errors.push_back(oss_.str());
                oss_.str("");
            };

            //Exit tasks
            switch (state_)
            {
                case State::Idle:
                    check_new_state(State::FlipBits);
                    break;
                case State::FlipBits:
                    check_new_state(State::Size);
                    break;
                case State::Size:
                    check_new_state(State::RawSDU);
                    break;
                case State::RawSDU:
                    check_new_state(State::SDU);
                    break;
                case State::SDU:
                    check_new_state(State::Idle);
                    break;
                case State::Error:
                    break;
                default: break;
            }

            const auto prev_state = state_;
            state_ = new_state;

            //Enter tasks
            switch (state_)
            {
                case State::FlipBits:
                    if (som_.empty())
                        //Without SOM, there are no FlipBits either
                        return change_state_(State::Size);
                    flip_bits_.resize(0);
                    break;
                case State::Size:
                    size_ = 0;
                    break;
                case State::RawSDU:
                    sdu_.resize(size_);
                    sdu_ix_ = 0;
                    if (size_ == 0)
                        //Empty message should be processed immediately since there will not come an actual RawSDU byte
                        return change_state_(State::SDU);
                    break;
                case State::SDU:
                    received_sdus.push_back(sdu_);
                    return change_state_(State::Idle);
                    break;
                case State::Error:
                    oss_ << "Error occured in state " << hr(prev_state);
                    errors.push_back(oss_.str());
                    oss_.str("");
                    return change_state_(State::Idle);
                    break;
                default: break;
            }
        }

        const std::string som_;
        my::Detector detector_;
        my::Detector flipped_detector_;
        State state_ = State::Idle;
        std::vector<bool> flip_bits_;
        std::size_t size_;
        SDU sdu_;
        std::size_t sdu_ix_;
        std::ostringstream oss_;
    };

} } 

#endif

#ifndef HEADER_gubg_wrap_my_Detector_hpp_ALREADY_INCLUDED
#define HEADER_gubg_wrap_my_Detector_hpp_ALREADY_INCLUDED

#include <string>
#include <cassert>

namespace gubg { namespace wrap { namespace my { 

    class Detector
    {
    public:
        //Assumes all sequence bytes are different
        Detector(const std::string &sequence): sequence_(sequence), som_front_(sequence.empty()?'\0':sequence[0])
        {
            reset();
        }

        bool found_sequence() const {return found_sequence_;}

        void reset()
        {
            match_len_ = 0;
            //An empty sequence always matches
            found_sequence_ = sequence_.empty();
        }

        void operator()(char ch)
        {
            if (sequence_.empty())
                return;

            found_sequence_ = false;

            auto check_for_sequence_match = [&]()
            {
                if (match_len_ == sequence_.size())
                {
                    //Complete sequence was found
                    match_len_ = 0;
                    found_sequence_ = true;
                }
            };

            //Check for sequence restart always
            if (ch == som_front_)
            {
                //Found start of sequence
                match_len_ = 1;

                check_for_sequence_match();
            }
            else if (match_len_ > 0)
            {
                //At least one byte of the sequence was found

                assert(match_len_ < sequence_.size());
                if (sequence_[match_len_] == ch)
                {
                    //Continuation of sequence
                    ++match_len_;

                    check_for_sequence_match();
                }
                else
                {
                    //Sequence was broken, restart at the beginning
                    reset();
                }
            }
        }

    private:
        const std::string sequence_;
        const char som_front_;

        std::size_t match_len_;
        bool found_sequence_;
    };

} } } 

#endif

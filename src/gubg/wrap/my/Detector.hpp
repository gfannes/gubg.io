#ifndef HEADER_gubg_wrap_my_Detector_hpp_ALREADY_INCLUDED
#define HEADER_gubg_wrap_my_Detector_hpp_ALREADY_INCLUDED

#include <string>
#include <cassert>

namespace gubg { namespace wrap { namespace my { 

    class Detector
    {
    public:
        Detector(const std::string &sequence): sequence_(sequence)
        {
            reset();
        }

        bool found_sequence() const {return found_sequence_;}

        void reset()
        {
            match_ix_ = 0;
            //An empty sequence always matches
            found_sequence_ = sequence_.empty();
        }

        void operator()(char ch)
        {
            if (sequence_.empty())
                return;

            found_sequence_ = false;

            assert(match_ix_ < sequence_.size());
            if (sequence_[match_ix_] != ch)
            {
                //Sequence was broken, restart at the beginning
                match_ix_ = 0;
            }
            else
            {
                ++match_ix_;

                if (match_ix_ == sequence_.size())
                {
                    //Completet sequence was followed correctly
                    match_ix_ = 0;
                    found_sequence_ = true;
                }
            }
        }

    private:
        const std::string sequence_;
        std::size_t match_ix_;

        bool found_sequence_;
    };

} } } 

#endif

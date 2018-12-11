#ifndef HEADER_gubg_t2_Segmenter_hpp_ALREADY_INCLUDED
#define HEADER_gubg_t2_Segmenter_hpp_ALREADY_INCLUDED

#include "gubg/t2/Span.hpp"
#include "gubg/mss.hpp"

namespace gubg { namespace t2 { 

    class Segmenter
    {
    public:
        Segmenter(Byte *begin, Byte *end): all_(begin, end) {}

        template <typename Ftor>
        bool process(Byte byte, Ftor &&ftor)
        {
            MSS_BEGIN(bool);
            switch (byte)
            {
                case md_som:
                    //A new segment starts
                    next_ = all_.begin();
                    break;
                case md_eom:
                    if (!!next_)
                    {
                        //A message was fully received
                        MSS(ftor(all_.begin(), next_));
                        next_ = nullptr;
                    }
                    break;
                default:
                    MSS(!!next_);
                    MSS(next_ != all_.end());
                    *next_++ = byte;
                    break;
            }
            MSS_END();
        }

    private:
        Span all_;
        Byte *next_ = nullptr;
    };

} } 

#endif

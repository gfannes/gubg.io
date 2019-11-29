#ifndef HEADER_gubg_wav_clamp_hpp_ALREADY_INCLUDED
#define HEADER_gubg_wav_clamp_hpp_ALREADY_INCLUDED

#include <algorithm>

namespace gubg { namespace wav { 

    inline float clamp24(float flt)
    {
        static const float pow23 = 8388608;
        static const float left_inc = -pow23/pow23;
        static const float right_inc = (pow23-1)/pow23;
        return std::max(left_inc, std::min(right_inc, flt));
    }

} } 

#endif

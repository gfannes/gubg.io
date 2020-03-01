#ifndef HEADER_gubg_wrap_my_som_hpp_ALREADY_INCLUDED
#define HEADER_gubg_wrap_my_som_hpp_ALREADY_INCLUDED

#include <string>

namespace gubg { namespace wrap { namespace my { 

    inline std::string flipped(const std::string &som)
    {
        std::string flipped_som = som;
        if (!flipped_som.empty())
        {
            auto &last = flipped_som.back();
            last = ~last;
        }
        return flipped_som;
    }

} } } 

#endif

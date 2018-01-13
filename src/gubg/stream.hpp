#ifndef HEADER_gubg_stream_hpp_ALREADY_INCLUDED
#define HEADER_gubg_stream_hpp_ALREADY_INCLUDED

#include <sstream>
#include <string>

namespace gubg {

    template <typename Functor>
    std::string stream(Functor && f)
    {
        std::ostringstream oss;
        f(oss);
        return oss.str();
    }

    template <typename Functor>
    void stream(std::string & str, Functor && f)
    {
        std::ostringstream oss;
        f(oss);
        str += oss.str();
    }
}

#endif

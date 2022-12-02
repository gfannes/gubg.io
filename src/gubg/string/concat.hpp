#ifndef HEADER_gubg_string_concat_hpp_ALREAD_INCLUDED
#define HEADER_gubg_string_concat_hpp_ALREAD_INCLUDED

#include <gubg/hr.hpp>

#include <string>

namespace gubg { namespace string {

    namespace my {
        inline void concat(std::string &str) {}

        template<typename Arg, typename... Rest>
        void concat(std::string &str, Arg arg, Rest... rest)
        {
            str += hr(arg);
            concat(str, rest...);
        }
    } // namespace my

    template<typename... Args>
    std::string concat(Args... args)
    {
        std::string str;
        my::concat(str, args...);
        return str;
    }

}} // namespace gubg::string

#endif

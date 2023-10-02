#ifndef HEADER_gubg_string_concat_hpp_ALREAD_INCLUDED
#define HEADER_gubg_string_concat_hpp_ALREAD_INCLUDED

#include <gubg/hr.hpp>

#include <string>
#include <sstream>

namespace gubg { namespace string {

    namespace my {
        inline void concat(std::string &str) {}

        template<typename Arg, typename... Rest>
        void concat(std::string &str, Arg arg, Rest... rest)
        {
            thread_local std::ostringstream oss;
            oss.str();
            oss << arg;
            str += oss.str();
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

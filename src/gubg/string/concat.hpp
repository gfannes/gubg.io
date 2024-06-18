#ifndef HEADER_gubg_string_concat_hpp_ALREADY_INCLUDED
#define HEADER_gubg_string_concat_hpp_ALREADY_INCLUDED

#include <gubg/hr.hpp>

#include <sstream>
#include <string>

namespace gubg { namespace string {

    namespace my {
        inline void concat(std::string &str) {}

        inline std::ostringstream &oss_meyers()
        {
            thread_local std::ostringstream oss;
            oss.str("");
            return oss;
        }

        template<typename Arg, typename... Rest>
        void concat(std::string &str, Arg arg, Rest... rest)
        {
            auto &oss = oss_meyers();
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

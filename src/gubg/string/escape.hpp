#ifndef HEADER_gubg_string_escape_hpp_ALREADY_INCLUDED
#define HEADER_gubg_string_escape_hpp_ALREADY_INCLUDED

#include <string>

namespace gubg { namespace string { 

    std::string escape_c(const std::string &str)
    {
        std::string res(str.size(), '?');
        res.resize(0);
        for (auto ch: str)
        {
            if (ch == '\\')
                res.push_back('\\');
            res.push_back(ch);
        }
        return res;
    }

    std::string escape_cmake(const std::string &str)
    {
        std::string res(str.size(), '?');
        res.resize(0);
        for (auto ch: str)
        {
            if (ch == '\\')
                res.push_back('/');
            else
                res.push_back(ch);
        }
        return res;
    }

} } 

#endif

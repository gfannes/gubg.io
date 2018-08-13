#ifndef HEADER_gubg_string_escape_hpp_ALREADY_INCLUDED
#define HEADER_gubg_string_escape_hpp_ALREADY_INCLUDED

#include <string>

namespace gubg { namespace string { 

    inline std::string escape_c(const std::string &str)
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

    inline std::string escape_cmake(const std::string &str, bool add_quotes = true)
    {
        std::string res(str.size()+(add_quotes ? 2 : 0), '?');
        res.resize(0);
        if (add_quotes) res.push_back('\"');
        for (auto ch: str)
        {
            if (ch == '\\')
                res.push_back('/');
            else
                res.push_back(ch);
        }
        if (add_quotes) res.push_back('\"');
        
        return res;
    }

    inline std::string dequote(const std::string & str)
    {
        if (str.size() >=2  && str.front() == '\"' && str.back() == '\"')
            return str.substr(1, str.size()-2);
        else
            return str;
    }

} } 

#endif

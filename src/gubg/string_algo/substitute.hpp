#ifndef HEADER_gubg_string_algo_substitute_hpp_ALREADY_INCLUDED
#define HEADER_gubg_string_algo_substitute_hpp_ALREADY_INCLUDED

namespace gubg { namespace string_algo { 

    template <typename String>
    void substitute(String &dst, String src, const String &pattern, const String &replace)
    {
        dst.clear();
        for (size_t pos = 0; true; )
        {
            auto pattern_pos = src.find(pattern, pos);
            if (pattern_pos == String::npos)
            {
                dst.append(src, pos, String::npos);
                return;
            }
            dst.append(src, pos, pattern_pos-pos);
            pos = pattern_pos;
            pos += pattern.size();
            dst.append(replace);
        }
    }

} } 

#endif

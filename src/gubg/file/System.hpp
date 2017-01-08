#ifndef HEADER_gubg_file_System_hpp_ALREADY_INCLUDED
#define HEADER_gubg_file_System_hpp_ALREADY_INCLUDED

#include "gubg/file/Name.hpp"
#include "gubg/mss.hpp"
#include "gubg/std/filesystem.hpp"
#include <regex>
#include <fstream>

namespace gubg { namespace file { 

    inline bool exists(const gubg::file::Name &fn)
    {
        MSS_BEGIN(bool);
        std::ifstream fi(fn);
        MSS(fi.good());
        MSS_END();
    }

    inline bool is_hidden(const std::filesystem::path &p)
    {
        const auto fn = p.filename();
        if (!fn.empty())
            return fn.c_str()[0] == '.';
        return false;
    }

    template <typename Callback>
        bool each_entry(Callback cb, const std::filesystem::path &dir, bool skip_hidden = true)
        {
            MSS_BEGIN(bool);
            for (const auto &path: std::filesystem::directory_iterator(dir))
            {
                if (skip_hidden && is_hidden(path))
                    continue;
                if (!cb(path))
                    break;
            }
            MSS_END();
        }
    template <typename Callback>
        bool each_entry(Callback cb) { return each_entry(cb, std::filesystem::current_path()); }

    template <typename Callback>
        bool each_recursive(Callback cb, const std::filesystem::path &dir, bool skip_hidden = true)
        {
            MSS_BEGIN(bool);
            auto recursor = [&](const std::filesystem::path &fn)
            {
                if (cb(fn) && std::filesystem::is_directory(fn))
                    each_recursive(cb, fn, skip_hidden);
                return true;
            };
            each_entry(recursor, dir, skip_hidden);
            MSS_END();
        }
    template <typename Callback>
        bool each_recursive(Callback cb) { return each_recursive(cb, std::filesystem::current_path()); }

    template <typename Callback>
        bool each_regex(const std::string &pattern, Callback cb)
        {
            MSS_BEGIN(bool, "");
            auto pattern_full = std::filesystem::current_path();
            pattern_full /= pattern;
            L(C(pattern_full));
            std::regex re(static_cast<std::string>(pattern_full));
            auto recursor = [&](const std::filesystem::path &fn)
            {
                const auto fn_str = static_cast<std::string>(fn);
                if (std::regex_match(fn_str, re))
                    cb(fn);
                return true;
            };
            each_recursive(recursor);
            MSS_END();
        }

    namespace details { 
        template <typename String>
            void sub(String &dst, String src, const String &pattern, const String &replace)
            {
                S("sub");
                dst.clear();
                for (size_t pos = 0; true; )
                {
                    L(C(pos));
                    auto pattern_pos = src.find(pattern, pos);
                    if (pattern_pos == String::npos)
                    {
                        L(C(pattern) << " not found (anymore)");
                        dst.append(src, pos, String::npos);
                        return;
                    }
                    L("Found " << C(pattern) << " at " << C(pattern_pos));
                    dst.append(src, pos, pattern_pos-pos);
                    pos = pattern_pos;
                    pos += pattern.size();
                    dst.append(replace);
                }
            }
    } 
    template <typename Callback>
        bool each_glob(const std::string &pattern, Callback cb)
        {
            std::string pattern_re = pattern;

            details::sub(pattern_re, pattern_re, std::string("."), std::string("\\."));
            details::sub(pattern_re, pattern_re, std::string("**"), std::string(".\0", 2));
            details::sub(pattern_re, pattern_re, std::string("*"), std::string("[^/]\0", 5));
            details::sub(pattern_re, pattern_re, std::string("\0", 1), std::string("*"));

            return each_regex(pattern_re, cb);
        }

} } 

#endif

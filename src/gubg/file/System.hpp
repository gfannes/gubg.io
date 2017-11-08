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
            if (std::filesystem::is_directory(dir))
            {
                for (const auto &path: std::filesystem::directory_iterator(dir))
                {
                    if (skip_hidden && is_hidden(path))
                        continue;
                    if (!cb(path))
                        break;
                }
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
        bool each_regex(const std::string &pattern, Callback cb, const std::filesystem::path &dir)
        {
            MSS_BEGIN(bool, "");
            auto pattern_full = dir;
            pattern_full /= pattern;
            L(C(pattern_full));
            std::regex re(pattern_full.string());
            auto recursor = [&](const std::filesystem::path &fn)
            {
                L(C(fn));
                if (std::regex_match(fn.string(), re))
                    cb(fn);
                return true;
            };
            each_recursive(recursor, dir);
            MSS_END();
        }
    template <typename Callback>
        bool each_regex(const std::string &pattern, Callback cb) { return each_regex(pattern, cb, std::filesystem::current_path()); }

    namespace details { 
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
    } 
    template <typename Callback>
        bool each_glob(const std::string &pattern, Callback cb, const std::filesystem::path &dir)
        {
            S("each_glob");L(C(pattern)C(dir));
            std::string pattern_re = pattern;

            details::substitute(pattern_re, pattern_re, std::string("."), std::string("\\."));
            //We use \0 to represent * temporarily
            details::substitute(pattern_re, pattern_re, std::string("**"), std::string(".\0", 2));
            details::substitute(pattern_re, pattern_re, std::string("*"), std::string("[^/]\0", 5));

            //Replace \0 with *
            details::substitute(pattern_re, pattern_re, std::string("\0", 1), std::string("*"));

            return each_regex(pattern_re, cb, dir);
        }
    template <typename Callback>
        bool each_glob(const std::string &pattern, Callback cb) { return each_glob(pattern, cb, std::filesystem::current_path()); }

} } 

#endif

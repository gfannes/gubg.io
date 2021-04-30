#ifndef HEADER_gubg_file_System_hpp_ALREADY_INCLUDED
#define HEADER_gubg_file_System_hpp_ALREADY_INCLUDED

#include "gubg/file/Name.hpp"
#include "gubg/mss.hpp"
#include "gubg/std/filesystem.hpp"
#include "gubg/string_algo/substitute.hpp"
#include <regex>
#include <fstream>

namespace gubg { namespace file { 

    inline bool read(std::string &content, const Name &name)
    {
        MSS_BEGIN(bool);
        std::ifstream fi(name.str(), std::ios::binary | std::ios::ate);
        MSS(fi.good());
        const auto end_pos = fi.tellg();
        fi.seekg(0);
        const size_t size = end_pos - fi.tellg();
        content.resize(size);
        fi.read(&content[0], size);
        MSS_END();
    }
    inline bool read(std::string &content, const std::filesystem::path &name) {return read(content, Name{name});}
    inline bool read(std::string &content, const std::string &name) {return read(content, Name{name});}

    inline bool write(const std::string &content, const Name &name)
    {
        MSS_BEGIN(bool);
        std::ofstream fo(name.str(), std::ios::binary);
        fo << content;
        MSS_END();
    }
    inline bool write(const std::string &content, const std::filesystem::path &name) {return write(content, Name{name});}
    inline bool write(const std::string &content, const std::string &name) {return write(content, Name{name});}

    inline void remove_empty_directories(const std::filesystem::path &dir)
    {
        for (auto entry: std::filesystem::directory_iterator{dir})
        {
            const auto fp = entry.path();

            if (!std::filesystem::is_directory(fp))
                continue;

            remove_empty_directories(fp);
        }

        std::error_code ec;
        std::filesystem::remove(dir, ec);
    }

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
        bool each_entry(Callback cb, const std::filesystem::path &dir, bool skip_hidden = true, bool skip_symlink = true)
        {
            MSS_BEGIN(bool);
            if (std::filesystem::is_directory(dir))
            {
                for (const auto &path: std::filesystem::directory_iterator(dir))
                {
                    if (skip_hidden && is_hidden(path))
                        continue;
                    if (skip_symlink && std::filesystem::is_symlink(path))
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

    //Recursive traversal of folder using std::regex
    template <typename Callback>
        bool each_regex(const std::regex &pattern_re, Callback cb, const std::filesystem::path &dir)
        {
            MSS_BEGIN(bool);

            auto recursor = [&](const std::filesystem::path &fn)
            {
                L(C(fn));
                if (std::regex_match(fn.string(), pattern_re))
                {
                    L("\tthis file matches the search pattern");
                    MSS(cb(fn));
                }
                return true;
            };
            each_recursive(recursor, dir);

            MSS_END();
        }
    //Start in current folder
    template <typename Callback>
        bool each_regex(const std::regex &pattern_re, Callback cb) { return each_regex(pattern_re, cb, std::filesystem::current_path()); }

    //Recursive traversal of folder using std::string
    template <typename Callback>
        bool each_regex(const std::string &pattern_str, Callback cb, std::filesystem::path dir)
        {
            MSS_BEGIN(bool);

            //Add trailing '/'
            dir += std::filesystem::path::preferred_separator;

            //Create consistent separators
            dir.make_preferred();

            //Remove all './', '../' and double separators
            std::string pattern_full_str = dir.lexically_normal().string();

            pattern_full_str += pattern_str;

            L(C(pattern_full_str));
            std::regex re(pattern_full_str);

            MSS(each_regex(re, cb, dir));

            MSS_END();
        }
    //Start in current folder
    template <typename Callback>
        bool each_regex(const std::string &pattern_str, Callback cb) { return each_regex(pattern_str, cb, std::filesystem::current_path()); }

    template <typename Callback>
        bool each_glob(const std::string &pattern_str, Callback cb, const std::filesystem::path &dir)
        {
            MSS_BEGIN(bool);
            L(C(pattern_str));

            std::string pattern_re = pattern_str;

            if (std::filesystem::path::preferred_separator == '\\')
                string_algo::substitute(pattern_re, pattern_re, std::string("/"), std::string("\\\\"));
            string_algo::substitute(pattern_re, pattern_re, std::string("."), std::string("\\."));
            //We use \0 to represent * temporarily
            string_algo::substitute(pattern_re, pattern_re, std::string("**"), std::string(".\0", 2));
            string_algo::substitute(pattern_re, pattern_re, std::string("*"), std::string("[^/\\\\]\0", 7));

            //Replace \0 with *
            string_algo::substitute(pattern_re, pattern_re, std::string("\0", 1), std::string("*"));
            L(C(pattern_str)C(pattern_re)C(dir));

            MSS(each_regex(pattern_re, cb, dir));

            MSS_END();
        }
    template <typename Callback>
        bool each_glob(const std::string &pattern_str, Callback cb) { return each_glob(pattern_str, cb, std::filesystem::current_path()); }

} } 

#endif

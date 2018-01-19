#ifndef HEADER_gubg_std_filesystem_hpp_ALREADY_INCLUDED
#define HEADER_gubg_std_filesystem_hpp_ALREADY_INCLUDED

#include "gubg/Range.hpp"
#include <experimental/filesystem>

namespace std { namespace filesystem { 

#define L_IMPORT(name) using name = std::experimental::filesystem::name
L_IMPORT(path);
L_IMPORT(directory_iterator);
#undef L_IMPORT

inline path current_path() {return std::experimental::filesystem::current_path();}
inline bool is_directory(const path &p) {return std::experimental::filesystem::is_directory(p);}
inline bool is_regular_file(const path &p) {return std::experimental::filesystem::is_regular_file(p);}
inline bool exists(const path & p) { return std::experimental::filesystem::exists(p); }
inline bool create_directory(const path &p) {return std::experimental::filesystem::create_directory(p);}
inline bool create_directories(const path &p) {return std::experimental::filesystem::create_directories(p);}
inline bool remove(const path &p) {return std::experimental::filesystem::remove(p);}
inline bool remove_all(const path &p) {return std::experimental::filesystem::remove_all(p);}
inline bool equivalent(const path &lhs, const path &rhs) {return std::experimental::filesystem::equivalent(lhs, rhs);}

} } 

namespace gubg{ namespace filesystem {

inline std::filesystem::path get_relative_to(const std::filesystem::path & from, const std::filesystem::path & to)
{
    using range = gubg::Range<std::filesystem::path::iterator>;

    auto pop_equal_fronts = [&](range & lhs, range & rhs)
    {
        while(!lhs.empty() && !rhs.empty())
        {
            if (lhs.front() != rhs.front())
                return;

            lhs.pop_front();
            rhs.pop_front();
        }
    };

    // from absolute and to relative -> to
    // from relative and to absolute -> to
    if (from.is_absolute() != to.is_absolute())
        return to;

    // remove the front part which is the same
    auto rf = gubg::make_range(from);
    auto rt = gubg::make_range(to);
    pop_equal_fronts(rf, rt);

    // start at from
    std::filesystem::path result;

    // go back till different part
    for(const auto & v : rf)
        result /= "..";
    // add the new part
    for(const auto & v : rt)
        result /= v;

    return result;
}

inline std::filesystem::path combine(const std::initializer_list<std::filesystem::path> & lst)
{
    auto it = lst.end();
    for(--it; it->is_relative() && it != lst.begin(); --it)
        ;;
    std::filesystem::path p;
    for(; it != lst.end(); ++it)
        p /= *it;
    return p;
}

inline std::filesystem::path combine(const std::filesystem::path & lhs, const std::filesystem::path & rhs)
{
    return rhs.is_absolute() ? rhs : lhs/rhs;
}

} }

#endif

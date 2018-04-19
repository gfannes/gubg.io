#ifndef HEADER_gubg_std_filesystem_hpp_ALREADY_INCLUDED
#define HEADER_gubg_std_filesystem_hpp_ALREADY_INCLUDED

#include "gubg/Range.hpp"
#include <experimental/filesystem>

namespace std { namespace filesystem { 

#define L_IMPORT(name) using name = std::experimental::filesystem::name
L_IMPORT(path);
L_IMPORT(directory_iterator);
L_IMPORT(recursive_directory_iterator);
L_IMPORT(directory_options);
#undef L_IMPORT

inline path current_path() {return std::experimental::filesystem::current_path();}
inline void current_path(const std::experimental::filesystem::path & path ) {std::experimental::filesystem::current_path(path);}
inline bool is_directory(const path &p) {return std::experimental::filesystem::is_directory(p);}
inline bool is_regular_file(const path &p) {return std::experimental::filesystem::is_regular_file(p);}
inline bool exists(const path & p) { return std::experimental::filesystem::exists(p); }
inline bool create_directory(const path &p) {return std::experimental::filesystem::create_directory(p);}
inline bool create_directories(const path &p) {return std::experimental::filesystem::create_directories(p);}
inline bool remove(const path &p) {return std::experimental::filesystem::remove(p);}
inline bool remove_all(const path &p) {return std::experimental::filesystem::remove_all(p);}
inline bool equivalent(const path &lhs, const path &rhs) {return std::experimental::filesystem::equivalent(lhs, rhs);}
inline std::size_t hash_value(const path & p) { return std::experimental::filesystem::hash_value(p); }


} } 

namespace std {

    template<> struct hash<std::filesystem::path>
    {
        typedef std::filesystem::path argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& s) const noexcept
        {
            return filesystem::hash_value(s);
        }
    };
}

namespace gubg{ namespace filesystem {

struct ChangeDirectory
{
    ChangeDirectory(const std::filesystem::path & new_directory)
        : old_(std::filesystem::current_path())
    {
        std::filesystem::current_path(new_directory);
    }

    ~ChangeDirectory()
    {
        std::filesystem::current_path(old_);
    }

    ChangeDirectory(ChangeDirectory &&) = default;
    ChangeDirectory & operator=(ChangeDirectory &&) = default;


private:
    ChangeDirectory(const ChangeDirectory &) = delete;
    ChangeDirectory & operator=(const ChangeDirectory &) = delete;


    std::filesystem::path old_;
};

inline ChangeDirectory change_directory(const std::filesystem::path & p)
{
    return ChangeDirectory(p);
}

std::filesystem::path get_relative_to(const std::filesystem::path & from, const std::filesystem::path & to);
std::filesystem::path normalize(const std::filesystem::path & src);

inline std::filesystem::path combine(const std::initializer_list<std::filesystem::path> & lst)
{
    auto it = lst.end();
    for(--it; it->is_relative() && it != lst.begin(); --it)
        ;;

    std::filesystem::path p;
    for(; it != lst.end(); ++it)
        p /= *it;

    return normalize(p);
}

inline std::filesystem::path combine(const std::filesystem::path & lhs, const std::filesystem::path & rhs)
{
    return normalize(rhs.is_absolute() ? rhs : lhs/rhs);
}



} }

#endif

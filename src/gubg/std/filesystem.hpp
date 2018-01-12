#ifndef HEADER_gubg_std_filesystem_hpp_ALREADY_INCLUDED
#define HEADER_gubg_std_filesystem_hpp_ALREADY_INCLUDED

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
    inline bool remove(const path &p) {return std::experimental::filesystem::remove(p);}
    inline bool remove_all(const path &p) {return std::experimental::filesystem::remove_all(p);}

} } 

#endif

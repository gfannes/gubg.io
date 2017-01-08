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

} } 

#endif

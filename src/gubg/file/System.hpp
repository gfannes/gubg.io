#ifndef HEADER_gubg_file_System_hpp_ALREADY_INCLUDED
#define HEADER_gubg_file_System_hpp_ALREADY_INCLUDED

#include "gubg/file/Name.hpp"
#include "gubg/mss.hpp"
#include <fstream>

namespace gubg { namespace file { 

    inline bool exists(const gubg::file::Name &fn)
    {
        MSS_BEGIN(bool);
        std::ifstream fi(fn);
        MSS(fi.good());
        MSS_END();
    }

} } 

#endif

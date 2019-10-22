#ifndef HEADER_gubg_sedes_Index_hpp_ALREADY_INCLUDED
#define HEADER_gubg_sedes_Index_hpp_ALREADY_INCLUDED

#include <vector>
#include <ostream>

namespace gubg { namespace sedes { 

    struct Index
    {
        unsigned int member_ix = 0;
        unsigned int array_ix = 0;
    };
    inline std::ostream &operator<<(std::ostream &os, const Index &index)
    {
        return os << "[index](this:" << &index << ")(mix:" << index.member_ix << ")(aix:" << index.array_ix << ")";
    }

    using Stack = std::vector<Index>;

} } 

#endif

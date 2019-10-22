#ifndef HEADER_gubg_sedes_Index_hpp_ALREADY_INCLUDED
#define HEADER_gubg_sedes_Index_hpp_ALREADY_INCLUDED

#include <list>
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

    //If this is replaced with a std::vector<>, rework the TRY macros: descending into a dfs() might change the location of the previous Index
    using Stack = std::list<Index>;

} } 

#endif

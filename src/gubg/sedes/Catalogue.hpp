#ifndef HEADER_gubg_sedes_Catalogue_hpp_ALREADY_INCLUDED
#define HEADER_gubg_sedes_Catalogue_hpp_ALREADY_INCLUDED

#include <gubg/sedes/Struct.hpp>
#include <gubg/mss.hpp>
#include <string>
#include <map>

namespace gubg { namespace sedes { 

    class Catalogue
    {
    public:
        bool add(const Struct &struc)
        {
            MSS_BEGIN(bool);
            MSS(name__struct_.count(struc.name) == 0);
            name__struct_[struc.name] = struc;
            MSS_END();
        }

        template <typename Ftor>
        void each(Ftor &&ftor)
        {
            for (auto p: name__struct_)
                ftor(p.second);
        }

    private:
        std::map<std::string, Struct> name__struct_;
    };

} } 

#endif

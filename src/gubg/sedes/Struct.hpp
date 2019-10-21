#ifndef HEADER_gubg_sedes_Struct_hpp_ALREADY_INCLUDED
#define HEADER_gubg_sedes_Struct_hpp_ALREADY_INCLUDED

#include <gubg/sedes/Member.hpp>
#include <string>
#include <vector>

namespace gubg { namespace sedes { 

    class Struct
    {
    public:
        std::string name;

        Struct() {}
        Struct(const std::string &name): name(name) {}

        void add(const Member::Ptr &ptr) { members_.push_back(ptr); }

        template <typename Ftor>
        void each(Ftor &&ftor) const
        {
            for (const auto &ptr: members_)
                ftor(*ptr);
        }

    private:
        std::vector<Member::Ptr> members_;
    };

} } 

#endif

#ifndef HEADER_gubg_sedes_naft_Reader_hpp_ALREADY_INCLUDED
#define HEADER_gubg_sedes_naft_Reader_hpp_ALREADY_INCLUDED

#include <gubg/mss.hpp>

namespace gubg { namespace sedes { namespace naft { 

    class Reader
    {
    public:
        Reader(const std::string &content)
        {
        }

        template <typename Obj>
        bool enter(Obj &obj, const std::string &name, const std::string &type)
        {
            MSS_BEGIN(bool);
            MSS_END();
        }
        template <typename Obj>
        bool exit(Obj &obj, const std::string &name, const std::string &type)
        {
            MSS_BEGIN(bool);
            MSS_END();
        }

        template <typename Obj>
        bool leaf(Obj &obj, const std::string &name, const std::string &type)
        {
            MSS_BEGIN(bool);
            MSS_END();
        }

        template <typename Obj>
        bool optional(Obj &obj, bool oc, const std::string &name, const std::string &type)
        {
            MSS_BEGIN(bool);
            MSS_END();
        }

        template <typename Obj>
        bool array(Obj &obj, bool oc, const std::string &name, const std::string &type)
        {
            MSS_BEGIN(bool);
            MSS_END();
        }
    private:
    };

} } } 

#endif

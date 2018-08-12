#ifndef HEADER_gubg_s11n_hpp_ALREADY_INCLUDED
#define HEADER_gubg_s11n_hpp_ALREADY_INCLUDED

#include "gubg/s11n/Reader.hpp"
#include "gubg/s11n/Writer.hpp"
#include "gubg/mss.hpp"

namespace gubg { namespace s11n { 

    template <typename String, typename Tag, typename Obj>
    bool write_object(String &str, const Tag &tag, const Obj &obj)
    {
        MSS_BEGIN(bool);
        s11n::Writer<String> w(str);
        MSS(w.object(tag, obj));
        MSS_END();
    }

    template <typename String, typename Tag, typename Obj>
    bool read_object(const String &str, const Tag &tag, Obj &obj)
    {
        MSS_BEGIN(bool);
        s11n::Reader r(str);
        MSS(r.object(tag, obj));
        MSS_END();
    }

} } 

#endif

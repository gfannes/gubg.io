#ifndef HEADER_gubg_s11n_Writer_hpp_ALREADY_INCLUDED
#define HEADER_gubg_s11n_Writer_hpp_ALREADY_INCLUDED

#include "gubg/s11n/Codes.hpp"
#include "gubg/s11n/StringAdapter.hpp"

namespace gubg { namespace s11n { 

    namespace details { 
        //push_back
        template <typename String>
        ReturnCode push_back(String &dst, char ch)
        {
            return dst.push_back(ch) ? ReturnCode::OK : ReturnCode::NoSpaceLeft;
        }

        //append
        template <typename String>
        ReturnCode append(String &dst, const char *src)
        {
            for (; *src; ++src)
                if (!dst.push_back(*src))
                    return ReturnCode::NoSpaceLeft;
            return ReturnCode::OK;
        }
    } 

    template <typename Tag_, typename String_>
    class Writer
    {
    public:
        using Tag = Tag_;
        using String = String_;

        Writer(String &str): str_(str) {}

        template <typename Obj>
        ReturnCode typed(const Obj &obj)
        {
            MSS_BEGIN(ReturnCode);
            MSS_END();
        }

        template <typename Obj, typename Name>
        ReturnCode named(const Obj &obj, const Name &name)
        {
            MSS_BEGIN(ReturnCode);
            MSS(details::push_back(str_, '['));
            MSS(details::append(str_, name));
            MSS(details::push_back(str_, ']'));
            MSS_END();
        }

    private:
        StringAdapter<String> str_;
    };

} } 

#endif

#ifndef HEADER_gubg_cpp_Struct_hpp_ALREADY_INCLUDED
#define HEADER_gubg_cpp_Struct_hpp_ALREADY_INCLUDED

#include <string>

namespace gubg { namespace cpp { 

    class SourceCode;

    class Struct
    {
    public:
        Struct(SourceCode &sc, const std::string &name);
        Struct(Struct &&dying);
        ~Struct();

        Struct &define_variable(const std::string &type, const std::string &name);

    private:
        SourceCode *sc_{};
    };

} } 

#endif

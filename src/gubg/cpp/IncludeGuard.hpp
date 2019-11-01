#ifndef HEADER_gubg_cpp_IncludeGuard_hpp_ALREADY_INCLUDED
#define HEADER_gubg_cpp_IncludeGuard_hpp_ALREADY_INCLUDED

#include <string>

namespace gubg { namespace cpp { 

    class SourceCode;

    class IncludeGuard
    {
    public:
        static std::string canonicalize(std::string fn);

        IncludeGuard(SourceCode &sc, const std::string &filename);
        IncludeGuard(IncludeGuard &&dying);
        ~IncludeGuard();

    private:
        SourceCode *sc_{};
    };

} } 

#endif

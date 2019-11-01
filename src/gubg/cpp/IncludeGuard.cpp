#include <gubg/cpp/IncludeGuard.hpp>
#include <gubg/cpp/SourceCode.hpp>

namespace gubg { namespace cpp { 

    std::string IncludeGuard::canonicalize(std::string fn)
    {
        for (auto &ch: fn)
            switch (ch)
            {
                case '/':
                case '.':
                case '-':
                case ' ':
                    ch = '_';
                    break;
            }
        return fn;
    }

    IncludeGuard::IncludeGuard(SourceCode &sc, const std::string &filename): sc_(&sc)
    {
        const auto filename_canon = canonicalize(filename);
        sc << "#ifndef HEADER_" << filename_canon << "_ALREADY_INCLUDED\n";
        sc << "#define HEADER_" << filename_canon << "_ALREADY_INCLUDED\n";
        sc << "\n";
    }
    IncludeGuard::IncludeGuard(IncludeGuard &&dying): sc_(dying.sc_)
    {
        dying.sc_ = nullptr;
    }
    IncludeGuard::~IncludeGuard()
    {
        if (!sc_)
            return;
        auto &sc = *sc_;
        sc << "\n";
        sc << "#endif\n";
    }

} } 

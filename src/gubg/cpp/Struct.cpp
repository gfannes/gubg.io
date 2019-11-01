#include <gubg/cpp/Struct.hpp>
#include <gubg/cpp/SourceCode.hpp>

namespace gubg { namespace cpp { 

    Struct::Struct(SourceCode &sc, const std::string &name): sc_(&sc)
    {
        sc.indent() << "struct " << name << "\n";
        sc.indent() << "{\n";
        sc.update_level(1);
    }
    Struct::Struct(Struct &&dying): sc_(dying.sc_)
    {
        dying.sc_ = nullptr;
    }
    Struct::~Struct()
    {
        if (!sc_)
            return;
        auto &sc = *sc_;
        sc.update_level(-1);
        sc.indent() << "};\n";
    }

    Struct &Struct::define_variable(const std::string &type, const std::string &name)
    {
        auto &sc = *sc_;
        sc.indent() << type << " " << name << "{};\n";
        return *this;
    }

} } 

#include <gubg/cpp/Function.hpp>
#include <gubg/cpp/SourceCode.hpp>

namespace gubg { namespace cpp { 

    Function::Function(SourceCode &sc, const std::string &name, const std::string &returntype, const std::string &args): sc_(&sc)
    {
        sc.indent() << returntype << " " << name << "(" << args << ")\n";
        sc.indent() << "{\n";
        sc.update_level(1);
    }
    Function::Function(Function &&dying): sc_(dying.sc_)
    {
        dying.sc_ = nullptr;
    }
    Function::~Function()
    {
        if (!sc_)
            return;
        auto &sc = *sc_;
        sc.update_level(-1);
        sc << "}\n";
    }

    Function &Function::define_variable(const std::string &type, const std::string &name)
    {
        auto &sc = *sc_;
        sc.indent() << type << " " << name << "{};\n";
        return *this;
    }

    Function &Function::statement(const std::string &str)
    {
        auto &sc = *sc_;
        sc.indent() << str << ";\n";
        return *this;
    }
    Function &Function::newline()
    {
        sc_->newline();
        return *this;
    }

    Function &Function::line(const std::string &str)
    {
        auto &sc = *sc_;
        sc.indent() << str << "\n";
        return *this;
    }

    void Function::update_level(int delta)
    {
        sc_->update_level(delta);
    }

} } 

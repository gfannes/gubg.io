#ifndef HEADER_gubg_cpp_SourceCode_hpp_ALREADY_INCLUDED
#define HEADER_gubg_cpp_SourceCode_hpp_ALREADY_INCLUDED

#include <gubg/cpp/IncludeGuard.hpp>
#include <gubg/cpp/Struct.hpp>
#include <gubg/cpp/Function.hpp>
#include <ostream>

namespace gubg { namespace cpp { 

    class SourceCode
    {
    public:
        SourceCode(std::ostream &os): os_(os) {}

        IncludeGuard include_guard(const std::string &filename) { return std::move(IncludeGuard{*this, filename}); }
        Struct define_struct(const std::string &name) { return std::move(Struct{*this, name}); }
        Function define_function(const std::string &name, const std::string &returntype, const std::string &args) { return std::move(Function{*this, name, returntype, args}); }

        SourceCode &include(const std::string &filename)
        {
            os_ << "#include <" << filename << ">" << std::endl;
            return *this;
        }

        SourceCode &newline()
        {
            os_ << std::endl;
            return *this;
        }

        SourceCode &declare_struct(const std::string &name)
        {
            indent() << "struct " << name << ";\n";
            return *this;
        }

        SourceCode &add_template(const std::string &args)
        {
            indent() << "template <" << args << ">\n";
            return *this;
        }

        SourceCode &indent()
        {
            os_ << std::string(level_*4, ' ');
            return *this;
        }

        template <typename T>
        SourceCode &operator<<(const T &v)
        {
            os_ << v;
            return *this;
        }

        void update_level(int delta) { level_ += delta; }

    private:
        std::ostream &os_;
        unsigned int level_ = 0;
    };

} } 

#endif

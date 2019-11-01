#ifndef HEADER_gubg_cpp_Function_hpp_ALREADY_INCLUDED
#define HEADER_gubg_cpp_Function_hpp_ALREADY_INCLUDED

#include <string>
#include <sstream>

namespace gubg { namespace cpp { 

    class SourceCode;

    class Function
    {
    public:
        Function(SourceCode &sc, const std::string &name, const std::string &returntype, const std::string &args);
        Function(Function &&dying);
        ~Function();

        Function &define_variable(const std::string &type, const std::string &name);
        Function &statement(const std::string &str);
        template <typename Ftor>
        Function &statement_stream(Ftor &&ftor)
        {
            oss_.str("");
            ftor(oss_);
            return statement(oss_.str());
        }
        Function &newline();

        Function &line(const std::string &str);

        template <typename Ok>
        Function &add_if(const std::string &condition, Ok &&ok)
        {
            oss_.str("");
            oss_ << "if (" << condition << ")";
            line(oss_.str());
            line("{");
            update_level(1);
            ok();
            update_level(-1);
            line("}");
            return *this;
        }

        template <typename Ftor>
        Function &add_switch(const std::string &v, Ftor &&ftor)
        {
            oss_.str("");
            oss_ << "switch (" << v << ")";
            line(oss_.str());
            line("{");
            update_level(1);
            ftor();
            update_level(-1);
            line("}");
            return *this;
        }

        template <typename T, typename Ftor>
        Function &add_case(const T &v, Ftor &&ftor)
        {
            oss_.str("");
            oss_ << "case " << v << ":";
            line(oss_.str());
            update_level(1);
            ftor();
            update_level(-1);
            return *this;
        }

        template <typename Ftor>
        Function &add_default(Ftor &&ftor)
        {
            line("default:");
            update_level(1);
            ftor();
            update_level(-1);
            return *this;
        }

        template <typename Ftor>
        Function &add_for(const std::string &a, const std::string &b, const std::string &c, Ftor &&ftor)
        {
            oss_.str("");
            oss_ << "for (" << a << "; " << b << "; " << c << ")";
            line(oss_.str());
            line("{");
            update_level(1);
            ftor();
            update_level(-1);
            line("}");
            return *this;
        }

        void update_level(int delta);

    private:
        SourceCode *sc_{};
        std::ostringstream oss_;
    };

} } 

#endif

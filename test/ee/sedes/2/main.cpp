#include <gubg/mss.hpp>
#include <gubg/file/System.hpp>
#include <gubg/sedes/naft/Reader.hpp>
#include <gubg/sedes/naft/Writer.hpp>
#include <abc.hpp>
#include <string>
#include <fstream>

inline std::ostream &indent(std::ostream &os, unsigned level) {for (auto i = 0; i < level; ++i) os << "  "; return os;}
inline void hr(std::ostream &os, const C &c, unsigned int level = 0)
{
    indent(os, level++) << "[:C]{\n";
    indent(os, level) << "[f_int](value:" << c.f_int << ")\n";
    indent(os, --level) << "}\n";
}
inline void hr(std::ostream &os, const B &b, unsigned int level = 0)
{
    indent(os, level++) << "[:B]{\n";
    indent(os, level) << "[f_int](value:" << b.f_int << ")\n";
    for (const auto &c: b.a_c)
        hr(indent(os, level), c, level);
    indent(os, --level) << "}\n";
}
inline void hr(std::ostream &os, const A &a, unsigned int level = 0)
{
    indent(os, level++) << "[:A]{\n";

    indent(os, level) << "[f_bool](value:" << a.f_bool << ")\n";
    indent(os, level) << "[f_int](value:" << a.f_int << ")\n";
    indent(os, level) << "[f_float](value:" << a.f_float << ")\n";
    indent(os, level) << "[f_char](value:" << (int)a.f_char << ")\n";
    hr(os, a.f_b, level);

    if (a.o_bool)
        indent(os, level) << "[o_bool](value:" << *a.o_bool << ")\n";
    if (a.o_int)
        indent(os, level) << "[o_int](value:" << *a.o_int << ")\n";
    if (a.o_float)
        indent(os, level) << "[o_float](value:" << *a.o_float << ")\n";
    if (a.o_char)
        indent(os, level) << "[o_char](value:" << (int)*a.o_char << ")\n";
    if (a.o_b)
        hr(os, *a.o_b, level);

    for (const auto &v: a.a_bool)
        indent(os, level) << "[a_bool](value:" << v << ")\n";
    for (const auto &v: a.a_int)
        indent(os, level) << "[a_int](value:" << v << ")\n";
    for (const auto &v: a.a_float)
        indent(os, level) << "[a_float](value:" << v << ")\n";
    for (const auto &v: a.a_char)
        indent(os, level) << "[a_char](value:" << (int)v << ")\n";

    for (const auto &b: a.a_b)
        hr(os, b, level);

    indent(os, --level) << "}\n";
}

int main(int argc, const char **argv)
{
    MSS_BEGIN(int);
    unsigned int ix = 0;
    auto pop = [&]() -> std::string
    {
        if (ix >= argc)
            return "";
        return argv[ix++];
    };

    pop();

    A a;
    B b;
    C c;

    for (; ix < argc;)
    {
        const auto arg = pop();
        if (false) {}
        else if (arg == "create")
        {
            const auto what = pop();
            if (false) {}
            else if (what == "empty") {}
            else if (what == "optional")
            {
                a.o_bool = true;
                a.o_int = 42;
                a.o_float = 4.2;
                a.o_char = '*';
                a.o_b.emplace();
                a.o_b->f_int = 43;
            }
            else if (what == "array")
            {
                a.a_bool.push_back(true);
                a.a_bool.push_back(false);
                a.a_int.push_back(42);
                a.a_int.push_back(41);
                a.a_int.push_back(40);
                a.a_float.push_back(0.1);
                a.a_float.push_back(0.2);
                a.a_float.push_back(0.3);
                a.a_char.push_back('a');
                a.a_char.push_back('b');
                a.a_char.push_back('c');
                a.a_b.emplace_back();
                a.a_b.back().f_int = 0;
                a.a_b.emplace_back();
                a.a_b.back().f_int = 1;
            }
        }
        else if (arg == "naft.read")
        {
            std::string content;
            MSS(gubg::file::read(content, pop()));
            gubg::sedes::naft::Reader reader{content};
            MSS(dfs(a, "", reader));
        }
        else if (arg == "naft.write")
        {
            std::ofstream os{pop()};
            gubg::sedes::naft::Writer writer{os};
            MSS(dfs(a, "", writer));
        }
        else if (arg == "hr.write")
        {
            std::ofstream os{pop()};
            hr(os, a);
        }
        else
        {
            MSS(false, std::cout << "Error: unknown argument " << arg << "\n");
        }
    }

    MSS_END();
}

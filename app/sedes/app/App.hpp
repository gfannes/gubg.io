#ifndef HEADER_app_App_hpp_ALREADY_INCLUDED
#define HEADER_app_App_hpp_ALREADY_INCLUDED

#include <app/output.hpp>
#include <app/Options.hpp>
#include <gubg/sedes/Catalogue.hpp>
#include <gubg/file/System.hpp>
#include <gubg/naft/Range.hpp>
#include <gubg/macro/capture.hpp>
#include <gubg/mss.hpp>
#include <fstream>

namespace app { 

    class App: private Options
    {
    public:
        bool process(int argc, const char **argv) { return Options::parse(argc, argv); }

        bool run()
        {
            MSS_BEGIN(bool);

            if (print_help)
            {
                log() << help();
                return true;
            }

            MSS(!input_fn.empty(), error() << "No input filename given\n");
            MSS(!output_fn.empty(), error() << "No output filename given\n");

            std::string content;
            MSS(gubg::file::read(content, input_fn));

            gubg::naft::Range range0{content};
            for (std::string name0,type0; range0.pop_tag(name0,type0); )
            {
                const auto attrs0 = range0.pop_attrs();
                log() << C(name0)C(type0)C(attrs0.size()) << std::endl;

                MSS(type0 == "struct");
                gubg::sedes::Struct struc{name0};

                gubg::naft::Range range1; 
                if (range0.pop_block(range1))
                {
                    for (std::string name1,type1; range1.pop_tag(name1,type1); )
                    {
                        const auto attrs1 = range1.pop_attrs();
                        log() << C(name1)C(type1)C(attrs1.size()) << std::endl;

                        auto set_common_info = [&](gubg::sedes::Member &member){
                            member.name = name1;
                            {
                                auto it = attrs1.find("type");
                                if (it != attrs1.end())
                                    member.type = it->second;
                            }
                        };
                        gubg::sedes::Member::Ptr member_ptr;
                        if (false) {}
                        else if (type1 == "field")
                        {
                            member_ptr = gubg::sedes::Field::create([&](auto &f){set_common_info(f);});
                        }
                        else if (type1 == "optional")
                        {
                            member_ptr = gubg::sedes::Optional::create([&](auto &o){set_common_info(o);});
                        }
                        else if (type1 == "array")
                        {
                            member_ptr = gubg::sedes::Array::create([&](auto &a){set_common_info(a);});
                        }
                        struc.add(member_ptr);
                    }
                }

                MSS(catalogue_.add(struc));
            }

            std::ofstream fo{output_fn};
            const auto output_fn_canon = canonicalize_(output_fn);
            fo << "#ifndef HEADER_" << output_fn_canon << "_ALREADY_INCLUDED\n";
            fo << "#define HEADER_" << output_fn_canon << "_ALREADY_INCLUDED\n";
            fo << "\n";
            fo << "#include <gubg/Tribool.hpp>\n";
            fo << "#include <gubg/sedes/Index.hpp>\n";
            fo << "#include <optional>\n";
            fo << "#include <vector>\n";
            fo << "#include <algorithm>\n";
            fo << "#include <iostream>\n";
            fo << "\n";
            catalogue_.each([&](const auto &struc){ declare_struct_(fo, struc); });
            fo << "\n";
            catalogue_.each([&](const auto &struc){ define_struct_(fo, struc); });
            fo << "\n";
            define_try_macros_(fo);
            fo << "\n";
            catalogue_.each([&](const auto &struc){ define_dfs_(fo, struc); });
            fo << "\n";
            fo << "#endif\n";

            MSS_END();
        }

    private:
        void declare_struct_(std::ostream &os, const gubg::sedes::Struct &struc)
        {
            os << "struct " << struc.name << ";\n";
        }
        void define_struct_(std::ostream &os, const gubg::sedes::Struct &struc)
        {
            using namespace gubg::sedes;
            class AddField: public Visitor
            {
            public:
                std::ostream &os;
                AddField(std::ostream &os): os(os) {}

                void visit(const Field &field) override
                {
                    os << "    " << field.type << ' ' << field.name << "{};\n";
                }
                void visit(const Optional &optional) override
                {
                    os << "    std::optional<" << optional.type << "> " << optional.name << ";\n";
                }
                void visit(const Array &array) override
                {
                    os << "    std::vector<" << array.type << "> " << array.name << ";\n";
                }
            };
            AddField add_field{os};

            os << "struct " << struc.name << "\n";
            os << "{\n";
            struc.each([&](const auto &member){member.accept(add_field);});
            os << "};\n";
        }
        void define_try_macros_(std::ostream &os)
        {
            os << "#define TRY1(my_mix, oper) {\\\n";
            os << "        if (stack[six].member_ix == my_mix){ \\\n";
            os << "            std::cout << \"  running\" << std::endl; \\\n";
            os << "            const gubg::Tribool res = oper; \\\n";
            os << "            if (!res.is_true()) return res; \\\n";
            os << "            ++stack[six].member_ix; \\\n";
            os << "        } \\\n";
            os << "    }\n";
            os << "#define TRY2(my_mix, my_aix, oper) {\\\n";
            os << "        if (stack[six].member_ix == my_mix && stack[six].array_ix == my_aix){ \\\n";
            os << "            std::cout << \"  running\" << std::endl; \\\n";
            os << "            const gubg::Tribool res = oper; \\\n";
            os << "            if (!res.is_true()) return res; \\\n";
            os << "            ++stack[six].array_ix; \\\n";
            os << "        } \\\n";
            os << "    }\n";
        }
        void define_dfs_(std::ostream &os, const gubg::sedes::Struct &struc)
        {
            using namespace gubg::sedes;
            class MyVisitor: public Visitor
            {
            public:
                std::ostream &os;
                unsigned int &mix;
                unsigned int &aix;
                MyVisitor(std::ostream &os, unsigned int &mix, unsigned int &aix): os(os), mix(mix), aix(aix) {}

                void visit(const Field &member) override
                {
                    if (member.is_primitive())
                        os << "    TRY1(" << mix++ << ", ftor.leaf(pod." << member.name << ", \"" << member.name << "\", \"" << member.type << "\"));\n";
                    else
                        os << "    TRY1(" << mix++ << ", dfs(pod." << member.name << ", \"" << member.name << "\", ftor, &stack, six));\n";
                }
                void visit(const Optional &member) override
                {
                    os << "    TRY1(" << mix++ << ", ftor.optional(pod." << member.name << ", true, \"" << member.name << "\", \"" << member.type << "\"));\n";
                    os << "    if (pod." << member.name << ")\n";
                    if (member.is_primitive())
                        os << "        TRY2(" << mix << ", 0, ftor.leaf(*pod." << member.name << ", \"" << member.name << "\", \"" << member.type << "\"));\n";
                    else
                        os << "        TRY2(" << mix << ", 0, dfs(*pod." << member.name << ", \"" << member.name << "\", ftor, &stack, six));\n";
                    os << "    TRY1(" << mix++ << ", (stack[six].array_ix=0,true));\n";
                    os << "    TRY1(" << mix++ << ", ftor.optional(pod." << member.name << ", false, \"" << member.name << "\", \"" << member.type << "\"));\n";
                }
                void visit(const Array &member) override
                {
                    os << "    TRY1(" << mix++ << ", ftor.array(pod." << member.name << ", true, \"" << member.name << "\", \"" << member.type << "\"));\n";
                    os << "    for (unsigned int ix = 0; ix < pod." << member.name << ".size(); ++ix)\n";
                    os << "    {\n";
                    if (member.is_primitive())
                    {
                        if (member.type == "bool")
                        {
                            //std::vector<bool> is specialized, we cannot take a reference of std::vector<bool>[ix]
                            os << "        bool v = pod." << member.name << "[ix];\n";
                            os << "        TRY2(" << mix << ", ix, ftor.leaf(v, \"" << member.name << "\", \"" << member.type << "\"));\n";
                            os << "        pod." << member.name << "[ix] = v;\n";
                        }
                        else
                        {
                            os << "        auto &v = pod." << member.name << "[ix];\n";
                            os << "        TRY2(" << mix << ", ix, ftor.leaf(v, \"" << member.name << "\", \"" << member.type << "\"));\n";
                        }
                    }
                    else
                    {
                        os << "        auto &v = pod." << member.name << "[ix];\n";
                        os << "        TRY2(" << mix << ", ix, dfs(v, \"" << member.name << "\", ftor, &stack, six));\n";
                    }
                    os << "    }\n";
                    os << "    TRY1(" << mix++ << ", (stack[six].array_ix=0,true));\n";
                    os << "    TRY1(" << mix++ << ", ftor.array(pod." << member.name << ", false, \"" << member.name << "\", \"" << member.type << "\"));\n";
                }
            };

            unsigned int mix = 0, aix = 0;
            MyVisitor my_visitor{os, mix, aix};

            os << "template <typename Ftor>\n";
            os << "gubg::Tribool dfs(" << struc.name << " &pod, const std::string &name, Ftor &&ftor, gubg::sedes::Stack *stack_ptr = nullptr, int six = -1)\n";
            os << "{\n";
            os << "    gubg::sedes::Stack local_stack;;\n";
            os << "    auto &stack = stack_ptr ? *stack_ptr : local_stack;\n";
            os << "    six++;\n";
            os << "    stack.resize(six+1);\n";
            os << "    TRY1(" << mix++ << ", ftor.enter(pod, name, \"" << struc.name << "\"));\n";
            os << "    \n";
            struc.each([&](const auto &member){
                    member.accept(my_visitor);
                    os << "    \n";
                    });
            os << "    TRY1(" << mix++ << ", ftor.exit(pod, name, \"" << struc.name << "\"));\n";
            os << "    stack.resize(six);\n";
            os << "    \n";
            os << "    return true;\n";
            os << "};\n";
        }

        static std::string canonicalize_(std::string fn)
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

        gubg::sedes::Catalogue catalogue_;
    };

} 

#endif

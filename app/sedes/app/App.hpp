#ifndef HEADER_app_App_hpp_ALREADY_INCLUDED
#define HEADER_app_App_hpp_ALREADY_INCLUDED

#include <app/output.hpp>
#include <app/Options.hpp>
#include <gubg/sedes/Catalogue.hpp>
#include <gubg/sedes/dsl/Parser.hpp>
#include <gubg/cpp/SourceCode.hpp>
#include <gubg/file/System.hpp>
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

            gubg::sedes::dsl::Parser parser{catalogue_};
            MSS(parser.process(content));

            std::ofstream fo{output_fn};
            gubg::cpp::SourceCode sc{fo};
            auto ig = sc.include_guard(output_fn);

            sc.include("gubg/Tribool.hpp");
            sc.include("gubg/mss.hpp");
            sc.include("string");
            sc.include("optional");
            sc.include("vector");

            catalogue_.each([&](const auto &struc){ define_struct_(sc, struc); });

            define_optional_dfs_(sc);

            define_array_dfs_(sc);

            catalogue_.each([&](const auto &struc){ define_dfs_(sc, struc); });

            MSS_END();
        }

    private:
        using SourceCode = gubg::cpp::SourceCode;

        void define_struct_(SourceCode &sc, const gubg::sedes::Struct &struc)
        {
            using namespace gubg::sedes;

            using Struct = gubg::cpp::Struct;

            class AddField: public Visitor
            {
            public:
                Struct &struc;
                AddField(Struct &struc): struc(struc) {}

                void visit(const Field &field) override
                {
                    struc.define_variable(field.type, field.name);
                }
                void visit(const Optional &optional) override
                {
                    struc.define_variable(std::string("std::optional<")+optional.type+">", optional.name);
                }
                void visit(const Array &array) override
                {
                    struc.define_variable(std::string("std::vector<")+array.type+">", array.name);
                }
            };

            sc.newline();
            Struct my_struct = sc.define_struct(struc.name);
            AddField add_field{my_struct};

            struc.each([&](const auto &member){
                    member.accept(add_field);
                    });
        }

        void define_optional_dfs_(SourceCode &sc)
        {
            for (auto i = 0; i < 2; ++i)
            {
                const auto is_leaf = (i == 0);

                sc.newline();
                sc.add_template("typename Obj, typename Ftor");
                auto func = sc.define_function((is_leaf ? "optional_leaf" : "optional_dfs"), "gubg::Tribool", "std::optional<Obj> &opt, const std::string &name, const std::string &type, Ftor &&ftor, std::vector<unsigned int> &stack, unsigned int ix");
                func.statement("MSS_BEGIN(gubg::Tribool)");

                func.newline();
                func.add_if("stack.size() == ix",
                        [&](){
                        func.statement("MSS(ftor.optional(opt, true, name, type))");
                        func.statement("stack.resize(ix+1)");
                        });

                func.newline();
                func.add_if("opt",
                        [&](){
                        if (is_leaf)
                        func.statement("MSS(ftor.leaf(*opt, name, type))");
                        else
                        func.statement("MSS(dfs(*opt, name, ftor, stack, ix+1))");
                        });

                func.newline();
                func.statement("MSS(ftor.optional(opt, false, name, type))");
                func.statement("stack.resize(ix)");

                func.newline();
                func.statement("MSS_END()");
            }
        }

        void define_array_dfs_(SourceCode &sc)
        {
            for (auto i = 0; i < 2; ++i)
            {
                const auto is_leaf = (i == 0);

                sc.newline();
                sc.add_template("typename Obj, typename Ftor");
                auto func = sc.define_function(is_leaf ? "array_leaf" : "array_dfs", "gubg::Tribool", "std::vector<Obj> &ary, const std::string &name, const std::string &type, Ftor &&ftor, std::vector<unsigned int> &stack, unsigned int ix");
                func.statement("MSS_BEGIN(gubg::Tribool)");

                func.newline();
                func.add_if("stack.size() == ix",
                        [&](){
                        func.statement("MSS(ftor.array(ary, true, name, type))");
                        func.statement("stack.resize(ix+1)");
                        });

                func.newline();
                func.add_for("auto i = stack[ix]", "i < ary.size()", "++i",
                        [&](){
                        if (is_leaf)
                        {
                        func.statement("Obj v = ary[i]");
                        func.statement("MSS(ftor.leaf(v, name, type))");
                        func.statement("ary[i] = v");
                        }
                        else
                        func.statement("MSS(dfs(ary[i], name, ftor, stack, ix+1))");
                        });

                func.newline();
                func.statement("MSS(ftor.array(ary, false, name, type))");
                func.statement("stack.resize(ix)");

                func.newline();
                func.statement("MSS_END()");
            }
        }

        void define_dfs_(SourceCode &sc, const gubg::sedes::Struct &struc)
        {
            using namespace gubg::sedes;
            using Function = gubg::cpp::Function;

            class MyVisitor: public Visitor
            {
            public:
                Function &func;
                std::ostringstream statement;

                MyVisitor(Function &func): func(func) {}

                void visit(const Field &member) override
                {
                    if (member.is_primitive())
                        statement << "ftor.leaf(pod." << member.name << ", \"" << member.name << "\", \"" << member.type << "\")";
                    else
                        statement << "dfs(pod." << member.name << ", \"" << member.name << "\", ftor, stack, ix+1)";
                }
                void visit(const Optional &member) override
                {
                    const auto function_name = (member.is_primitive() ? "optional_leaf" : "optional_dfs");
                    statement << function_name << "(pod." << member.name << ", \"" << member.name << "\", \"" << member.type << "\", ftor, stack, ix+1)";
                }
                void visit(const Array &member) override
                {
                    const auto function_name = (member.is_primitive() ? "array_leaf" : "array_dfs");
                    statement << function_name << "(pod." << member.name << ", \"" << member.name << "\", \"" << member.type << "\", ftor, stack, ix+1)";
                }
            };

            {
                sc.newline();
                sc.add_template("typename Ftor");
                auto func = sc.define_function("dfs", "gubg::Tribool", struc.name + " &pod, const std::string &name, Ftor &&ftor, std::vector<unsigned int> &stack, unsigned int ix");

                func.statement("MSS_BEGIN(gubg::Tribool)");

                int member_ix = -1;
                auto continue_with_next_member = [&](){
                    member_ix++;
                    func.statement_stream([&](auto &os){os << "stack[ix] = " << member_ix;});
                };

                func.newline();
                func.add_if("stack.size() == ix",
                        [&](){
                        func.statement("MSS(ftor.enter(pod, name, \"" + struc.name + "\"))");
                        func.statement("stack.resize(ix+1)");
                        continue_with_next_member();
                        });

                func.newline();
                func.add_switch("stack[ix]",
                        [&](){
                        MyVisitor my_visitor{func};
                        struc.each([&](const auto &member){
                                my_visitor.statement.str("");
                                member.accept(my_visitor);
                                func.add_case(member_ix,
                                        [&](){
                                        func.statement_stream([&](auto &os){os << "MSS(" << my_visitor.statement.str() << ")";});
                                        continue_with_next_member();
                                        });
                                });
                        func.add_case(member_ix,
                                [&](){
                                func.statement("break");
                                });
                        func.add_default([&](){
                                func.statement("MSS(false)");
                                func.statement("break");
                                });
                        });

                func.newline();
                func.statement("MSS(ftor.exit(pod, name, \"" + struc.name + "\"))");
                func.statement("stack.resize(ix)");

                func.newline();
                func.statement("MSS_END()");
            }

            {
                sc.newline();
                sc.add_template("typename Ftor");
                auto func = sc.define_function("dfs", "gubg::Tribool", struc.name + " &pod, const std::string &name, Ftor &&ftor");
                func.define_variable("std::vector<unsigned int>", "stack");
                func.statement("return dfs(pod, name, ftor, stack, 0)");
            }
        }

        gubg::sedes::Catalogue catalogue_;
    };

} 

#endif

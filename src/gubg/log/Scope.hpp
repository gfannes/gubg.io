#ifndef HEADER_gubg_log_Scope_hpp_ALREADY_INCLUDED
#define HEADER_gubg_log_Scope_hpp_ALREADY_INCLUDED

#include "gubg/log/Primitives.hpp"

namespace gubg { namespace log { 

    class Scope
    {
    public:
        Scope(const char *name)
        {
            auto &parent = parent_meyers_();
            if (!!parent)
                parent->open_block();
            print_indent_();
            print('[');
            print(name);
            print(']');
            ++level_();
            parent_ = parent;
            parent = this;
        }
        ~Scope()
        {
            --level_();
            parent_meyers_() = parent_;
            if (has_block_)
            {
                print_indent_();
                print('}');
            }
            println();
        }

        template <typename T>
        void attr(const char *name, const T &v)
        {
            print('(');
            print(name);
            print(':');
            print(v);
            print(')');
        }

        void open_block()
        {
            if (has_block_)
                return;
            print('{');
            println();
            has_block_ = true;
        }

    private:
        bool has_block_ = false;
        Scope *parent_ = nullptr;

        static Scope *&parent_meyers_()
        {
            static Scope *s_parent_ = nullptr;
            return s_parent_;
        }

        static unsigned int &level_()
        {
            static unsigned int s_level_ = 0;
            return s_level_;
        }
        static void print_indent_()
        {
            const auto level = level_();
            for (auto i = 0u; i < level; ++i)
                print("  ");
        }
    };

} } 

#endif

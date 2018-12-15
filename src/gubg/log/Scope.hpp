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
            {
                parent->open_block();
                prim_ = parent->prim_;
            }
            print_indent_();
            prim_.print('[');
            prim_.print(name);
            prim_.print(']');
            ++level_();
            parent_ = parent;
            parent = this;
        }
        template <typename Target>
        Scope(Target &target, const char *name)
        {
            prim_.setup(target);
            auto &parent = parent_meyers_();
            if (!!parent)
                parent->open_block();
            print_indent_();
            prim_.print('[');
            prim_.print(name);
            prim_.print(']');
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
                prim_.print('}');
            }
            prim_.println();
        }

        template <typename T>
        void attr(const char *name, const T &v)
        {
            prim_.print('(');
            prim_.print(name);
            prim_.print(':');
            prim_.print(v);
            prim_.print(')');
        }

        void open_block()
        {
            if (has_block_)
                return;
            prim_.print('{');
            prim_.println();
            has_block_ = true;
        }

    private:
        Primitive prim_;
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
        void print_indent_()
        {
            const auto level = level_();
            for (auto i = 0u; i < level; ++i)
                prim_.print("  ");
        }
    };

} } 

#endif

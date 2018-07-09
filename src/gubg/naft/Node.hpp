#ifndef HEADER_gubg_naft_Node_hpp_ALREADY_INCLUDED
#define HEADER_gubg_naft_Node_hpp_ALREADY_INCLUDED

#include <string>
#include <ostream>

namespace gubg { namespace naft {

    class Document;

    class Node
    {
    private:
        friend class Document;
        Node(std::ostream &os): level_(-1), os_(&os), is_leaf_(false) { }

    public:
        Node(int level, std::ostream &os, const std::string &name): level_(level), os_(&os)
        {
            indent_() << '[' << name << ']';
        }
        Node(Node &&dying): level_(dying.level_), os_(dying.os_), is_leaf_(dying.is_leaf_)
        {
            //Indicate this node is dead
            dying.os_ = nullptr;
        }
        ~Node()
        {
            if (!os_)
                //This node is dead
                return;

            if (!is_leaf_)
                indent_() << "}" << std::endl;
            else
                *os_ << std::endl;
        }

        Node node(const std::string &name)
        {
            if (is_leaf_)
            {
                *os_ << "{" << std::endl;
                is_leaf_ = false;
            }
            Node n(level_+1, *os_, name);
            do_indent_ = true;
            return n;
        }

        template <typename Key, typename Value>
        Node &attr(const Key &key, const Value &value)
        {
            *os_ << '(' << key << ':' << value << ')';
            return *this;
        }
        template <typename Key>
        Node &attr(const Key &key)
        {
            *os_ << '(' << key << ')';
            return *this;
        }

        template <typename T>
        Node &text(const T &v)
        {
            if (is_leaf_)
            {
                *os_ << "{";
                is_leaf_ = false;
            }
            *os_ << v;
            do_indent_ = false;
            return *this;
        }

    private:
        Node(const Node &);
        Node &operator=(const Node &);
        Node &operator=(Node &&dying);

        std::ostream &indent_()
        {
            if (do_indent_)
                *os_ << std::string(2*level_, ' ');
            return *os_;
        }

        const int level_;
        std::ostream *os_ = nullptr;
        bool is_leaf_ = true;
        bool do_indent_ = true;
    };

} } 

#endif

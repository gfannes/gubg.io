#ifndef HEADER_gubg_tree_Node_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tree_Node_hpp_ALREADY_INCLUDED

#include <string>
#include <ostream>

namespace gubg { namespace tree { 

    class Node
    {
    public:
        Node(int level, std::ostream &os, const std::string &name): level_(level), os_(&os)
        {
            indent_(false) << '[' << name << ']';
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
                indent_(false) << "}" << std::endl;
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
            return n;
        }

        template <typename Key, typename Value>
        Node &attr(const Key &key, const Value &value)
        {
            *os_ << '(' << key << ':' << value << ')';
            return *this;
        }

    private:
        Node(const Node &);
        Node &operator=(const Node &);
        Node &operator=(Node &&dying);

        std::ostream &indent_(bool newline = false)
        {
            if (newline)
                *os_ << std::endl;
            *os_ << std::string(2*level_, ' ');
            return *os_;
        }

        const int level_;
        std::ostream *os_ = nullptr;
        bool is_leaf_ = true;
    };

} } 

#endif

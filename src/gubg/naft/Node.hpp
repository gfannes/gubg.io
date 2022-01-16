#ifndef HEADER_gubg_naft_Node_hpp_ALREADY_INCLUDED
#define HEADER_gubg_naft_Node_hpp_ALREADY_INCLUDED

// Escaping:
// * Node: '[' => '[<]', ']' => '[>]', '{' => '{<}', '}' => '{>}'
// * Attr: '(' => '(<)', ')' => '(>)', '{' => '{<}', '}' => '{>}'
// * Text: '[' => '{(}', ']' => '{)}', '{' => '{<}', '}' => '{>}'

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
            auto &my_os = indent_() << '[';
            if (node_needs_escaping_(name))
                stream_escaped_node_(my_os, name);
            else
                my_os << name;
            my_os << ']';
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
            *os_ << '(';
            if (attr_needs_escaping_(key))
                stream_escaped_attr_(*os_, key);
            else
                *os_ << key;
            *os_ << ':';
            if (attr_needs_escaping_(value))
                stream_escaped_attr_(*os_, value);
            else
                *os_ << value;
            *os_ << ')';
            return *this;
        }
        template <typename Key>
        Node &attr(const Key &key)
        {
            *os_ << '(';
            if (attr_needs_escaping_(key))
                stream_escaped_attr_(*os_, key);
            else
                *os_ << key;
            *os_ << ')';
            return *this;
        }

        template <typename T>
        Node &text(const T &v)
        {
            if (is_leaf_)
            {
                *os_ << '{';
                is_leaf_ = false;
            }
            if (text_needs_escaping_(v))
                stream_escaped_text_(*os_, v);
            else
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

        template <typename T>
        bool node_needs_escaping_(const T &v) {return needs_escaping_<'[', ']'>(v);}
        template <typename T>
        bool attr_needs_escaping_(const T &v) {return needs_escaping_<'(', ')'>(v);}
        template <typename T>
        bool text_needs_escaping_(const T &) {return false;}
        bool text_needs_escaping_(const std::string &str)
        {
            int nesting_level_b = 0;

            for (auto ix = 0u; ix < str.size(); ++ix)
            {
                const auto ch = str[ix];
                switch (ch)
                {
                    case '[': return false; break;
                    case ']': return false; break;

                    case '{':
                    ++nesting_level_b;
                    if (str.size()-ix >= 3 && (str[ix+1] == '<' || str[ix+1] == '>') && str[ix+2] == '}')
                        //The original string contains the escape sequence {<} or {>}
                        return true;
                    break;

                    case '}':
                    --nesting_level_b;
                    if (nesting_level_b < 0)
                        //Nesting B goes below 0 due to reversed brackets
                        return true;
                    break;

                    default: break;
                }
            }

            const auto is_balanced_b = (nesting_level_b == 0);
            return !is_balanced_b;
        }

        template <char o, char c, typename T>
        bool needs_escaping_(const T &) {return false;}
        template <char o, char c>
        bool needs_escaping_(const std::string &str)
        {
            int nesting_level_a = 0, nesting_level_b = 0;

            for (auto ix = 0u; ix < str.size(); ++ix)
            {
                const auto ch = str[ix];
                switch (ch)
                {
                    case o:
                    ++nesting_level_a;
                    if (str.size()-ix >= 3 && (str[ix+1] == '<' || str[ix+1] == '>') && str[ix+2] == c)
                        //The original string contains the escape sequence o<c or o>c
                        return true;
                    break;

                    case c:
                    --nesting_level_a;
                    if (nesting_level_a < 0)
                        //Nesting A goes below 0 due to reversed brackets
                        return true;
                    break;

                    case '{':
                    ++nesting_level_b;
                    if (str.size()-ix >= 3 && (str[ix+1] == '<' || str[ix+1] == '>') && str[ix+2] == '}')
                        //The original string contains the escape sequence {<} or {>}
                        return true;
                    break;

                    case '}':
                    --nesting_level_b;
                    if (nesting_level_b < 0)
                        //Nesting B goes below 0 due to reversed brackets
                        return true;
                    break;

                    default: break;
                }
            }

            const auto is_balanced_a = (nesting_level_a == 0);
            const auto is_balanced_b = (nesting_level_b == 0);
            return !is_balanced_a || !is_balanced_b;
        }

        template <typename T>
        void stream_escaped_node_(std::ostream &os, const T &v) {return stream_escaped_<'[', ']'>(os, v);}
        template <typename T>
        void stream_escaped_attr_(std::ostream &os, const T &v) {return stream_escaped_<'(', ')'>(os, v);}
        void stream_escaped_text_(std::ostream &os, const std::string &str)
        {
            for (auto ch: str)
            {
                switch (ch)
                {
                    case '[': os << "{(}"; break;
                    case ']': os << "{)}"; break;
                    case '{': os << "{<}"; break;
                    case '}': os << "{>}"; break;
                    default: os << ch; break;
                }
            }
        }

        template <char o, char c, typename T>
        void stream_escaped_(std::ostream &os, const T &v) {os << v;}
        template <char o, char c>
        void stream_escaped_(std::ostream &os, const std::string &str)
        {
            for (auto ch: str)
            {
                switch (ch)
                {
                    case o: os << o << '<' << c; break;
                    case c: os << o << '>' << c; break;
                    case '{': os << "{<}"; break;
                    case '}': os << "{>}"; break;
                    default: os << ch; break;
                }
            }
        }

        const int level_;
        std::ostream *os_ = nullptr;
        bool is_leaf_ = true;
        bool do_indent_ = true;
    };

} } 

#endif

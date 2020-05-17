#ifndef HEADER_gubg_markdown_ast_Tree_hpp_ALREADY_INCLUDED
#define HEADER_gubg_markdown_ast_Tree_hpp_ALREADY_INCLUDED

#include <gubg/tree/Forest.hpp>
#include <string>

namespace gubg { namespace markdown { namespace ast { 

    enum class Type
    {
        Text, Title, List, Link,
    };

    struct Data
    {
        Type type = Type::Text;
        std::string content;
    };

    using Forest = gubg::tree::Forest<Data>;

} } } 

#endif

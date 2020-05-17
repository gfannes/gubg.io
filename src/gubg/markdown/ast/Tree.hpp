#ifndef HEADER_gubg_markdown_ast_Tree_hpp_ALREADY_INCLUDED
#define HEADER_gubg_markdown_ast_Tree_hpp_ALREADY_INCLUDED

#include <gubg/tree/Forest.hpp>
#include <string>

namespace gubg { namespace markdown { namespace ast { 

    enum class Type
    {
        Section, Title, Text, List, Link,
    };

    struct Data
    {
        Type type = Type::Text;
        std::string content;

        Data() {}
        Data(Type type): type(type) {}
        Data(Type type, const std::string &content): type(type), content(content) {}
    };

    using Forest = gubg::tree::Forest<Data>;

} } } 

#endif

#ifndef HEADER_gubg_markdown_ast_Writer_hpp_ALREADY_INCLUDED
#define HEADER_gubg_markdown_ast_Writer_hpp_ALREADY_INCLUDED

#include <gubg/markdown/ast/Tree.hpp>
#include <ostream>

namespace gubg { namespace markdown { namespace ast { 

    bool write(std::ostream &, const Forest &);

} } } 

#endif

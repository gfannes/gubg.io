#include <gubg/markdown/ast/Tree.hpp>
#include <gubg/markdown/ast/Writer.hpp>
#include <catch.hpp>
#include <sstream>
#include <iostream>
using namespace gubg::markdown;

TEST_CASE("markdown::ast::Tree tests", "[ut][markdown][ast][Tree]")
{
    ast::Forest forest;

    SECTION("title")
    {
       auto &doc = forest.append(ast::Type::Section);

       doc.childs.append(ast::Type::Title, "My title");
       doc.childs.append(ast::Type::Text, "text1");
       doc.childs.append(ast::Type::Text, "text2");
    }

    std::ostringstream oss;
    REQUIRE(ast::write(oss, forest));
    std::cout << oss.str();
}


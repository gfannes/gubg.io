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
       auto node = forest.find(forest.size(), true);
       REQUIRE(!!node);
       node->value.type = ast::Type::Title;
       node->value.content = "My title";

       auto node = forest.find(forest.size(), true);
    }

    std::ostringstream oss;
    REQUIRE(ast::write(oss, forest));
    std::cout << oss.str();
}


#include <gubg/string/concat.hpp>

#include <catch.hpp>

using namespace gubg;

TEST_CASE("Test string concatenation", "[ut][string][concat]")
{
    REQUIRE(string::concat() == "");
    REQUIRE(string::concat(42) == "42");
    REQUIRE(string::concat(4, 'a', 2, "bc") == "4a2bc");
}

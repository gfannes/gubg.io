#include <gubg/string/Buffer.hpp>
#include <gubg/Range_macro.hpp>
#include <catch.hpp>
#include <array>

namespace  { 
    using String = gubg::string::Buffer<char>;
} 

TEST_CASE("string::Buffer tests", "[ut][string][Buffer]")
{
    std::array<char, 4> buffer;
    String string{RANGE(buffer)};

    SECTION("fresh")
    {
        REQUIRE(string.empty());
        REQUIRE(string.size() == 0);
    }
    SECTION("grow(4)")
    {
        const auto ptr = string.grow(4);
        REQUIRE(!!ptr);
        REQUIRE(!string.empty());
        REQUIRE(string.size() == 4);
    }
    SECTION("grow(4,'?')")
    {
        const auto ptr = string.grow(4,'?');
        REQUIRE(!!ptr);
        REQUIRE(!string.empty());
        REQUIRE(string.size() == 4);
    }
}

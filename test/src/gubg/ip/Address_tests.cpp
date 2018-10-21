#include "catch.hpp"
#include "gubg/ip/Address.hpp"
#include <iostream>
using namespace gubg;

TEST_CASE("ip::Address tests", "[ut][ip][Address]")
{
    ip::Address address;
    SECTION("fresh")
    {
        ip::Address zero{0,0,0,0};
        REQUIRE(address == zero);
    }
    std::cout << address << std::endl;
}

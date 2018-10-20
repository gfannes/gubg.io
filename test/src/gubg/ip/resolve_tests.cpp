#include "catch.hpp"
#include "gubg/ip/resolve.hpp"
#include <iostream>
using namespace gubg;

TEST_CASE("ip::resolve() tests", "[ut][ip][resolve]")
{
    ip::Address address;
    SECTION("fresh")
    {
        REQUIRE(address == ip::Address{0,0,0,0});
    }
    SECTION("OK")
    {
        REQUIRE(ip::resolve(address, "fannes.com") == ip::ReturnCode::OK);
    }
    SECTION("UnkownHost")
    {
        REQUIRE(ip::resolve(address, "fannes.co") == ip::ReturnCode::UnkownHost);
        REQUIRE(address == ip::Address{0,0,0,0});
    }
    SECTION("localhost")
    {
        REQUIRE(ip::resolve(address, "localhost") == ip::ReturnCode::OK);
        REQUIRE(address == ip::Address{127,0,0,1});
    }
    std::cout << address << std::endl;
}

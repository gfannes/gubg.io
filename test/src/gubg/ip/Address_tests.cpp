#include "catch.hpp"
#include "gubg/ip/Address.hpp"
using namespace gubg;

TEST_CASE("ip::Address tests", "[ut][ip][Address]")
{
    ip::Address address;
    REQUIRE(!!address.as_sockaddr());
}

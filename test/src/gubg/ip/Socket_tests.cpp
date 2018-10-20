#include "catch.hpp"
#include "gubg/ip/Socket.hpp"
#include "gubg/macro/capture.hpp"
#include <iostream>
using namespace gubg;

TEST_CASE("ip::Socket tests", "[ut][ip][Socket]")
{
    ip::Socket socket;
    SECTION("fresh")
    {
        REQUIRE(!socket.valid());
    }
    SECTION("ctor")
    {
        ip::Socket tmp(ip::Type::UDP, ip::Version::V4);
        std::cout << C(tmp) << std::endl;
        socket = std::move(tmp);
        std::cout << C(tmp) << std::endl;
        REQUIRE(socket.valid());
    }
    SECTION("bind")
    {
        REQUIRE(socket.setup(ip::Type::UDP, ip::Version::V4));
        ip::Address address{127,0,0,1};
        REQUIRE(socket.bind(address, 1234) == ip::ReturnCode::OK);
    }
    std::cout << socket << std::endl;
}

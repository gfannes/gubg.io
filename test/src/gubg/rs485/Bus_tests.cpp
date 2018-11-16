#include "catch.hpp"
#include "gubg/rs485/Bus.hpp"
using namespace gubg;

TEST_CASE("rs485::Bus tests", "[ut][rs485][Bus]")
{
    rs485::Bus bus;

    auto a = bus.create_endpoint();
    auto b = bus.create_endpoint();
    auto c = bus.create_endpoint();

    a->send(std::byte{'a'});
    b->send(std::byte{'b'});
    c->send(std::byte{'c'});

    bus.process();

    std::byte byte;
    REQUIRE(a->receive(byte));
    REQUIRE(byte == std::byte{'b'});
    REQUIRE(a->receive(byte));
    REQUIRE(byte == std::byte{'c'});
    REQUIRE(!a->receive(byte));

    REQUIRE(b->receive(byte));
    REQUIRE(byte == std::byte{'a'});
    REQUIRE(b->receive(byte));
    REQUIRE(byte == std::byte{'c'});
    REQUIRE(!b->receive(byte));

    REQUIRE(c->receive(byte));
    REQUIRE(byte == std::byte{'a'});
    REQUIRE(c->receive(byte));
    REQUIRE(byte == std::byte{'b'});
    REQUIRE(!c->receive(byte));
}

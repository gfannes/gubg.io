#include <gubg/wrap/Encoder.hpp>
#include <gubg/hex.hpp>
#include <catch.hpp>
#include <iostream>
#include <iomanip>
using namespace gubg;

TEST_CASE("wrap::Encoder tests", "[wrap][Encoder]")
{
    wrap::SDU sdu;

    SECTION("empty") { }
    SECTION("abc") { sdu = "abc"; }

    wrap::Encoder encoder;

    wrap::PDU pdu;
    encoder(pdu, sdu);

    for (auto ch: pdu)
        std::cout << hex(ch) << ' ';
    std::cout << std::endl;

    REQUIRE(pdu.size() >= 6);
}

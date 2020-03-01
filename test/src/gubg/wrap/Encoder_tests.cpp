#include <gubg/wrap/Encoder.hpp>
#include <gubg/hex.hpp>
#include <catch.hpp>
#include <iostream>
#include <iomanip>
using namespace gubg;

TEST_CASE("wrap::Encoder tests", "[wrap][Encoder]")
{
    struct Scn
    {
        std::string som;
        wrap::SDU sdu;
    };

    Scn scn;

    SECTION("default") { }
    SECTION("som = SOM")
    {
        scn.som = "SOM";
        SECTION("data = ABC") { scn.sdu = "ABC"; }
        SECTION("data = SOM") { scn.sdu = "SOM"; }
        SECTION("data = SOMSOmSOM") { scn.sdu = "SOMSO\xb2SOM"; }
        SECTION("data = SOmSOMSOm") { scn.sdu = "SO\xb2SOMSO\xb2"; }
    }

    wrap::Encoder encoder{scn.som};

    wrap::PDU pdu;
    encoder(pdu, scn.sdu);

    for (auto ch: pdu)
        std::cout << hex(ch) << '-' << ch << ' ';
    std::cout << std::endl;

    REQUIRE(pdu.size() >= scn.som.size()+scn.sdu.size());
    if (!scn.som.empty())
    {
        const auto som_size = scn.som.size();
        std::size_t ix = 0;
        REQUIRE(pdu.substr(ix, som_size) == scn.som);
        const auto end_ix = pdu.size()-scn.som.size()+1;
        for (++ix; ix < end_ix; ++ix)
        {
            REQUIRE(pdu.substr(ix, som_size) != scn.som);
        }
    }
}

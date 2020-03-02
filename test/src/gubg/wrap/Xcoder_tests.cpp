#include <gubg/wrap/Encoder.hpp>
#include <gubg/wrap/Decoder.hpp>
#include <catch.hpp>
using namespace gubg;

TEST_CASE("wrap::Xcoder tests", "[wrap][Xcoder]")
{
    struct Scn
    {
        std::string som;
        wrap::SDU sdu;
        bool print = true;
    };

    Scn scn;

    SECTION("default") { }
    SECTION("som = SOM")
    {
        scn.som = "SOM";
        SECTION("data = ABC") { scn.sdu = "ABC"; }
        SECTION("data = SOM") { scn.sdu = "SOM"; }
        SECTION("data = SSOM") { scn.sdu = "SSOM"; }
        SECTION("data = SOSOM") { scn.sdu = "SOSOM"; }
        SECTION("data = SOSOMSSOM") { scn.sdu = "SOSOMSSOM"; }
        SECTION("data = SOMSOmSOM") { scn.sdu = "SOMSO\xb2SOM"; }
        SECTION("data = SOmSOMSOm") { scn.sdu = "SO\xb2SOMSO\xb2"; }
        SECTION("data = 1mb") { scn.sdu = std::string(1024*1024, '?'); scn.print = false; }
    }

    wrap::Encoder encoder{scn.som};
    wrap::PDU pdu;
    REQUIRE(encoder(pdu, scn.sdu));

    //Print PDU
    if (scn.print)
    {
        for (auto ch: pdu)
            std::cout << hex(ch) << '-' << ch << ' ';
        std::cout << std::endl;
    }

    //Check PDU has minimal size
    REQUIRE(pdu.size() >= scn.som.size()+scn.sdu.size());

    //Check PDU does not contain SOM
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

    wrap::Decoder decoder{scn.som};
    decoder.process(pdu);
    for (const auto &error: decoder.errors)
        std::cout << error << std::endl;

    REQUIRE(decoder.errors.empty());
    REQUIRE(decoder.received_sdus.size() == 1);
    REQUIRE(decoder.received_sdus[0] == scn.sdu);
}

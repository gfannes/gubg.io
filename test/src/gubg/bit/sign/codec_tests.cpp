#include <gubg/bit/sign/codec.hpp>
#include <catch.hpp>
using namespace gubg;

TEST_CASE("gubg::bit::sign::codec tests", "[ut][bit][sign][codec]")
{
    struct Scn
    {
        std::int8_t i = 0;
        std::uint8_t ui = 0;
    };

    Scn scn;

    SECTION("-128 255u") { scn.i = -128; scn.ui = 255u; }

    SECTION("  -2 3u")   { scn.i = -2;   scn.ui = 3u; }
    SECTION("  -1 1u")   { scn.i = -1;   scn.ui = 1u; }
    SECTION("   0 0u")   { scn.i = 0;    scn.ui = 0u; }
    SECTION("   1 2u")   { scn.i = 1;    scn.ui = 2u; }

    SECTION(" 127 254u") { scn.i = 127;  scn.ui = 254u; }

    REQUIRE(bit::sign::encode(scn.i)  == scn.ui);
    REQUIRE(bit::sign::decode(scn.ui) == scn.i);
}

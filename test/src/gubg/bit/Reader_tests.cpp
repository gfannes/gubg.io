#include <gubg/bit/Reader.hpp>
#include <catch.hpp>
using namespace gubg;

TEST_CASE("bit::Reader tests", "[ut][bit][Reader]")
{
    const char *bytes = "\xa1\x01";

    SECTION("constructor")
    {
        bit::Reader r{bytes, 2};
    }

    bit::Reader r;

    REQUIRE(r.setup(bytes, 2));

    SECTION("zero()/one()")
    {
        REQUIRE(r.one());
        REQUIRE(r.zero());
        REQUIRE(r.zero());
        REQUIRE(r.zero());

        REQUIRE(r.zero());
        REQUIRE(r.one());
        REQUIRE(r.zero());
        REQUIRE(r.one());

        REQUIRE(r.one());
    }
    SECTION("bit()")
    {
        bool b;
        b = false; r.bit(b); REQUIRE(b);
        b = true;  r.bit(b); REQUIRE(!b);
        b = true;  r.bit(b); REQUIRE(!b);
        b = true;  r.bit(b); REQUIRE(!b);

        b = true;  r.bit(b); REQUIRE(!b);
        b = false; r.bit(b); REQUIRE(b);
        b = true;  r.bit(b); REQUIRE(!b);
        b = false; r.bit(b); REQUIRE(b);

        b = false; r.bit(b); REQUIRE(b);
    }
    SECTION("uint()")
    {
        unsigned int ui;
        SECTION("4 4 8")
        {
            ui = 0u; r.uint(ui, 4); REQUIRE(ui == 1);
            ui = 0u; r.uint(ui, 4); REQUIRE(ui == 0xa);
            ui = 0u; r.uint(ui, 8); REQUIRE(ui == 1);
        }
        SECTION("9 7")
        {
            ui = 0u;    r.uint(ui, 9); REQUIRE(ui == 0x01a1);
            ui = 0xffu; r.uint(ui, 7); REQUIRE(ui == 0x0);
        }
    }
}
